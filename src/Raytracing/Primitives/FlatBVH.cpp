// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cassert>
#include <chrono>
#include <limits>
#include <stdexcept>

#include "Raytracing/Primitives/FlatBVH.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "App.h"
#include "Utils/Log.h"
#include "Math/Vector3.h"
#include "Math/EulerAngle.h"

using namespace Raycer;

void Raycer::FlatBVH::initialize(const Scene& scene)
{
	(void)scene;
}

bool FlatBVH::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
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
				if (orderedPrimitives[flatNode.startOffset + i]->intersect(ray, intersection, intersections))
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
	return aabb;
}

void FlatBVH::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	(void)scale;
	(void)rotate;
	(void)translate;
}

void FlatBVH::build(const std::vector<Primitive*>& primitives, const BVHBuildInfo& buildInfo, const Scene& scene)
{
	Log& log = App::getLog();

	if (hasBeenBuilt)
	{
		orderedPrimitives.clear();

		for (int primitiveId : orderedPrimitiveIds)
			orderedPrimitives.push_back(scene.primitivesMap.at(primitiveId));

		return;
	}
	else
	{
		flatNodes.clear();
		orderedPrimitiveIds.clear();
	}

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
			if (orderedPrimitives[i]->getAABB().getCenter().element(axis) <= splitPoint)
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

	hasBeenBuilt = true;
	aabb = flatNodes[0].aabb;

	for (const Primitive* primitive : orderedPrimitives)
		orderedPrimitiveIds.push_back(primitive->id);

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
		splitPoint = nodeAABB.getCenter().element(axis);
	else if (buildInfo.axisSplit == BVHAxisSplit::MEDIAN)
		splitPoint = calculateMedianPoint(axis, buildEntry);
	else if (buildInfo.axisSplit == BVHAxisSplit::RANDOM)
	{
		std::uniform_real_distribution<double> dist(nodeAABB.getMin().element(axis), nodeAABB.getMax().element(axis));
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
		double tempSplitPoint = nodeAABB.getCenter().element(tempAxis);
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
			double step = nodeAABB.getExtent().element(tempAxis) / (double)buildInfo.regularSAHSplits;
			tempSplitPoint = nodeAABB.getMin().element(tempAxis);

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

		if (primitiveAABB.getCenter().element(axis) <= splitPoint)
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
		score += (leftAABB.getSurfaceArea() / nodeAABB.getSurfaceArea()) * (double)leftCount;

	if (rightCount > 0)
		score += (rightAABB.getSurfaceArea() / nodeAABB.getSurfaceArea()) * (double)rightCount;

	return score;
}

double FlatBVH::calculateMedianPoint(int axis, const FlatBVHBuildEntry& buildEntry)
{
	std::vector<double> centerPoints;

	for (int i = buildEntry.start; i < buildEntry.end; ++i)
		centerPoints.push_back(orderedPrimitives[i]->getAABB().getCenter().element(axis));

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
