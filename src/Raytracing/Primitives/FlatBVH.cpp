// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "Raytracing/Primitives/FlatBVH.h"
#include "Raytracing/Primitives/BVH.h"
#include "Raytracing/Primitives/PrimitiveList.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "App.h"
#include "Utils/Log.h"

using namespace Raycer;

void FlatBVH::initialize()
{
}

bool FlatBVH::intersect(const Ray& ray, Intersection& intersection) const
{
	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	int stack[128];
	int stackptr = 0;
	bool wasFound = false;

	// push to stack
	stack[stackptr] = 0;
	stackptr++;

	while (stackptr > 0)
	{
		// pop from stack
		stackptr--;
		int index = stack[stackptr];
		FlatBVHNode flatNode = flatNodes[index];

		// leaf node -> intersect with all its primitives
		if (flatNode.rightOffset == 0)
		{
			for (int i = 0; i < flatNode.primitiveCount; ++i)
			{
				if (orderedPrimitives[flatNode.startOffset + i]->intersect(ray, intersection))
				{
					if (ray.fastOcclusion)
						return true;

					wasFound = true;
				}
			}
		}
		else // travel down the tree
		{
			// right child
			if (flatNodes[index + flatNode.rightOffset].aabb.intersects(ray))
			{
				stack[stackptr] = index + flatNode.rightOffset;
				stackptr++;
			}

			// left child
			if (flatNodes[index + 1].aabb.intersects(ray))
			{
				stack[stackptr] = index + 1;
				stackptr++;
			}
		}
	}

	return wasFound;
}

AABB FlatBVH::getAABB() const
{
	return flatNodes[0].aabb;
}

void FlatBVH::build(const std::vector<Primitive*>& primitives, const BVHBuildInfo& buildInfo)
{
	Log& log = App::getLog();

	log.logInfo("Building BVH (primitives: %d)", primitives.size());

	auto startTime = std::chrono::high_resolution_clock::now();

	std::random_device rd;
	std::mt19937 generator(rd());

	FlatBVHBuildEntry stack[128];
	orderedPrimitives = primitives;
	flatNodes.clear();

	int stackptr = 0;
	int nodeCount = 0;
	int leafCount = 0;
	int actualNodeCount = 0;

	enum { ROOT = -4, UNVISITED = -3, VISITED_TWICE = -1 };

	// push to stack
	stack[stackptr].start = 0;
	stack[stackptr].end = (int)orderedPrimitives.size();
	stack[stackptr].parent = ROOT;
	stackptr++;

	while (stackptr > 0)
	{
		stackptr--;
		nodeCount++;

		// pop from stack
		FlatBVHNode flatNode;
		FlatBVHBuildEntry buildEntry = stack[stackptr];
		flatNode.rightOffset = UNVISITED;
		flatNode.startOffset = buildEntry.start;
		flatNode.primitiveCount = buildEntry.end - buildEntry.start;

		for (int i = buildEntry.start; i < buildEntry.end; ++i)
			flatNode.aabb.expand(orderedPrimitives[i]->getAABB());

		flatNode.aabb.update();

		// leaf node indicated by rightOffset = 0
		if (flatNode.primitiveCount <= buildInfo.maxLeafSize)
		{
			flatNode.rightOffset = 0;
			leafCount++;
		}

		flatNodes.push_back(flatNode);

		// update the parent rightOffset when visiting its right child
		if (buildEntry.parent != ROOT)
		{
			flatNodes[buildEntry.parent].rightOffset++;

			if (flatNodes[buildEntry.parent].rightOffset == VISITED_TWICE)
				flatNodes[buildEntry.parent].rightOffset = nodeCount - 1 - buildEntry.parent;
		}

		// leaf node -> no further subdivision
		if (flatNode.rightOffset == 0)
			continue;

		int axis;
		double splitPoint;
		actualNodeCount++;

		if (buildInfo.useSAH)
			calculateSAHSplit(axis, splitPoint, flatNode.aabb, buildInfo, buildEntry);
		else
			calculateSplit(axis, splitPoint, flatNode.aabb, buildInfo, buildEntry, generator);

		int middle = buildEntry.start;

		// partition primitive range by the split point
		for (int i = buildEntry.start; i < buildEntry.end; ++i)
		{
			if (orderedPrimitives[i]->getAABB().center.element(axis) <= splitPoint)
			{
				std::swap(orderedPrimitives[i], orderedPrimitives[middle]);
				middle++;
			}
		}

		// partition failed -> fallback
		if (middle == buildEntry.start || middle == buildEntry.end)
			middle = buildEntry.start + (buildEntry.end - buildEntry.start) / 2;

		// push right child
		stack[stackptr].start = middle;
		stack[stackptr].end = buildEntry.end;
		stack[stackptr].parent = nodeCount - 1;
		stackptr++;

		// push left child
		stack[stackptr].start = buildEntry.start;
		stack[stackptr].end = middle;
		stack[stackptr].parent = nodeCount - 1;
		stackptr++;
	}

	auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
	int milliseconds = (int)std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();

	log.logInfo("BVH building finished (time: %d ms, nodes: %d, leafs: %d)", milliseconds, nodeCount, leafCount);
}

void FlatBVH::calculateSplit(int& axis, double& splitPoint, const AABB& nodeAABB, const BVHBuildInfo& buildInfo, const FlatBVHBuildEntry& buildEntry, std::mt19937& generator)
{
	if (buildInfo.axisSelection == BVHAxisSelection::LARGEST)
		axis = nodeAABB.getLargestAxis();
	else if (buildInfo.axisSelection == BVHAxisSelection::RANDOM)
	{
		std::uniform_int_distribution<int> dist(0, 2);
		axis = dist(generator);
	}
	else
		throw std::runtime_error("Unknown BVH axis selection");

	if (buildInfo.axisSplit == BVHAxisSplit::MIDDLE)
		splitPoint = nodeAABB.center.element(axis);
	else if (buildInfo.axisSplit == BVHAxisSplit::MEDIAN)
		splitPoint = calculateMedianPoint(axis, buildEntry);
	else if (buildInfo.axisSplit == BVHAxisSplit::RANDOM)
	{
		std::uniform_real_distribution<double> dist(nodeAABB.min.element(axis), nodeAABB.max.element(axis));
		splitPoint = dist(generator);
	}
	else
		throw std::runtime_error("Unknown BVH axis split");
}

void FlatBVH::calculateSAHSplit(int& axis, double& splitPoint, const AABB& nodeAABB, const BVHBuildInfo& buildInfo, const FlatBVHBuildEntry& buildEntry)
{
	double lowestScore = std::numeric_limits<double>::max();

	for (int tempAxis = 0; tempAxis <= 2; ++tempAxis)
	{
		double tempSplitPoint = nodeAABB.center.element(tempAxis);
		double score = calculateSAHScore(tempAxis, tempSplitPoint, nodeAABB, buildEntry);

		if (score < lowestScore)
		{
			axis = tempAxis;
			splitPoint = tempSplitPoint;
			lowestScore = score;
		}

		tempSplitPoint = calculateMedianPoint(tempAxis, buildEntry);
		score = calculateSAHScore(tempAxis, tempSplitPoint, nodeAABB, buildEntry);

		if (score < lowestScore)
		{
			axis = tempAxis;
			splitPoint = tempSplitPoint;
			lowestScore = score;
		}

		if (buildInfo.regularSAHSplits > 0)
		{
			double step = nodeAABB.extent.element(tempAxis) / (double)buildInfo.regularSAHSplits;
			tempSplitPoint = nodeAABB.min.element(tempAxis);

			for (int i = 0; i < buildInfo.regularSAHSplits - 1; ++i)
			{
				tempSplitPoint += step;
				score = calculateSAHScore(tempAxis, tempSplitPoint, nodeAABB, buildEntry);

				if (score < lowestScore)
				{
					axis = tempAxis;
					splitPoint = tempSplitPoint;
					lowestScore = score;
				}
			}
		}
	}
}

double FlatBVH::calculateSAHScore(int axis, double splitPoint, const AABB& nodeAABB, const FlatBVHBuildEntry& buildEntry)
{
	assert(buildEntry.end - buildEntry.start > 0);

	AABB leftAABB, rightAABB;
	int leftCount = 0;
	int rightCount = 0;

	for (int i = buildEntry.start; i < buildEntry.end; ++i)
	{
		AABB primitiveAABB = orderedPrimitives[i]->getAABB();

		if (primitiveAABB.center.element(axis) <= splitPoint)
		{
			leftAABB.expand(primitiveAABB);
			leftCount++;
		}
		else
		{
			rightAABB.expand(primitiveAABB);
			rightCount++;
		}
	}

	double score = 0.0;

	if (leftCount > 0)
	{
		leftAABB.update();
		score += (leftAABB.surfaceArea / nodeAABB.surfaceArea) * (double)leftCount;
	}

	if (rightCount > 0)
	{
		rightAABB.update();
		score += (rightAABB.surfaceArea / nodeAABB.surfaceArea) * (double)rightCount;
	}

	return score;
}

double FlatBVH::calculateMedianPoint(int axis, const FlatBVHBuildEntry& buildEntry)
{
	std::vector<double> centerPoints;

	for (int i = buildEntry.start; i < buildEntry.end; ++i)
		centerPoints.push_back(orderedPrimitives[i]->getAABB().center.element(axis));

	std::sort(centerPoints.begin(), centerPoints.end());
	int size = (int)centerPoints.size();
	double median = 0.0;

	assert(size >= 2);

	if (size % 2 == 0)
		median = (centerPoints[size / 2 - 1] + centerPoints[size / 2]) / 2.0;
	else
		median = centerPoints[size / 2];

	return median;
}
