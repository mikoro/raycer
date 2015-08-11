// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "Raytracing/Primitives/BVH.h"
#include "Raytracing/Primitives/PrimitiveList.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "App.h"
#include "Utils/Log.h"

using namespace Raycer;

void BVH::initialize()
{
}

void BVH::intersect(const Ray& ray, Intersection& intersection) const
{
	if (!aabb.intersects(ray))
		return;

	left->intersect(ray, intersection);
	right->intersect(ray, intersection);
}

AABB BVH::getAABB() const
{
	return aabb;
}

void BVH::construct(const std::vector<Primitive*>& primitives, BVH* node, const BVHInfo& info)
{
	Log& log = App::getLog();

	log.logInfo("Constructing the BVH (primitives: %d)", primitives.size());

	auto startTime = std::chrono::high_resolution_clock::now();
	std::random_device rd;
	std::mt19937 gen(rd());
	int nodeCount = 0;

	constructRecursive(primitives, node, info, gen, nodeCount, 0, 0, 0);

	auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
	int milliseconds = (int)std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();

	log.logInfo("BVH construction finished (time: %d ms, nodes: %d)", milliseconds, nodeCount);
}

void BVH::free(BVH* node)
{
	if (typeid(node->left) == typeid(BVH))
		free((BVH*)node->left);
	else
		delete node->left;

	if (typeid(node->right) == typeid(BVH))
		free((BVH*)node->right);
	else
		delete node->right;
}

void BVH::constructRecursive(const std::vector<Primitive*>& primitives, BVH* node, const BVHInfo& info, std::mt19937& gen, int& nodeCount, int previousLeftSize, int previousRightSize, int sameSizeCount)
{
	nodeCount++;

	for (Primitive* primitive : primitives)
		node->aabb.expand(primitive->getAABB());

	node->aabb.update();

	int axis;
	double divisionPoint;

	calculateSplit(axis, divisionPoint, primitives, node, info, gen);

	std::vector<Primitive*> leftPrimitives;
	std::vector<Primitive*> rightPrimitives;

	for (Primitive* primitive : primitives)
	{
		if (primitive->getAABB().center.element(axis) <= divisionPoint)
			leftPrimitives.push_back(primitive);
		else
			rightPrimitives.push_back(primitive);
	}

	int leftSize = (int)leftPrimitives.size();
	int rightSize = (int)rightPrimitives.size();
	bool shouldTerminate = false;

	if (leftSize == previousLeftSize && rightSize == previousRightSize)
	{
		if (++sameSizeCount >= 5)
			shouldTerminate = true;
	}
	else
		sameSizeCount = 0;

	previousLeftSize = leftSize;
	previousRightSize = rightSize;

	if (shouldTerminate)
		App::getLog().logWarning("One BVH node got stuck and was terminated early");

	if (leftPrimitives.size() > (size_t)info.maxLeafSize && !shouldTerminate)
	{
		node->left = new BVH();
		constructRecursive(leftPrimitives, (BVH*)node->left, info, gen, nodeCount, previousLeftSize, previousRightSize, sameSizeCount);
	}
	else
		node->left = new PrimitiveList(leftPrimitives);

	if (rightPrimitives.size() > (size_t)info.maxLeafSize && !shouldTerminate)
	{
		node->right = new BVH();
		constructRecursive(rightPrimitives, (BVH*)node->right, info, gen, nodeCount, previousLeftSize, previousRightSize, sameSizeCount);
	}
	else
		node->right = new PrimitiveList(rightPrimitives);
}

void BVH::calculateSplit(int& axis, double& divisionPoint, const std::vector<Primitive*>& primitives, BVH* node, const BVHInfo& info, std::mt19937& gen)
{
	if (info.useSAH)
	{
		calculateSAHSplit(axis, divisionPoint, primitives, node, info);
		return;
	}

	if (info.axisSelection == BVHAxisSelection::LARGEST)
		axis = node->aabb.getLargestAxis();
	else if (info.axisSelection == BVHAxisSelection::RANDOM)
	{
		std::uniform_int_distribution<int> dist(0, 2);
		axis = dist(gen);
	}
	else
		throw std::runtime_error("Unknown BVH axis selection");

	if (info.axisSplit == BVHAxisSplit::MIDDLE)
		divisionPoint = node->aabb.center.element(axis);
	else if (info.axisSplit == BVHAxisSplit::MEDIAN)
		divisionPoint = calculateMedianPoint(axis, primitives);
	else if (info.axisSplit == BVHAxisSplit::RANDOM)
	{
		std::uniform_real_distribution<double> dist(node->aabb.min.element(axis), node->aabb.max.element(axis));
		divisionPoint = dist(gen);
	}
	else
		throw std::runtime_error("Unknown BVH axis split");
}

void BVH::calculateSAHSplit(int& axis, double& divisionPoint, const std::vector<Primitive*>& primitives, BVH* node, const BVHInfo& info)
{
	double lowestScore = std::numeric_limits<double>::max();

	for (int tempAxis = 0; tempAxis <= 2; ++tempAxis)
	{
		double tempDivisionPoint = node->aabb.center.element(tempAxis);
		double score = calculateSAHScore(tempAxis, tempDivisionPoint, primitives, node);

		if (score < lowestScore)
		{
			axis = tempAxis;
			divisionPoint = tempDivisionPoint;
			lowestScore = score;
		}

		tempDivisionPoint = calculateMedianPoint(tempAxis, primitives);
		score = calculateSAHScore(tempAxis, tempDivisionPoint, primitives, node);

		if (score < lowestScore)
		{
			axis = tempAxis;
			divisionPoint = tempDivisionPoint;
			lowestScore = score;
		}

		if (info.regularSAHSplits > 0)
		{
			double step = node->aabb.extent.element(tempAxis) / (double)info.regularSAHSplits;
			tempDivisionPoint = node->aabb.min.element(tempAxis);

			for (int i = 0; i < info.regularSAHSplits - 1; ++i)
			{
				tempDivisionPoint += step;
				score = calculateSAHScore(tempAxis, tempDivisionPoint, primitives, node);

				if (score < lowestScore)
				{
					axis = tempAxis;
					divisionPoint = tempDivisionPoint;
					lowestScore = score;
				}
			}
		}
	}
}

double BVH::calculateSAHScore(int axis, double divisionPoint, const std::vector<Primitive*>& primitives, BVH* node)
{
	assert(primitives.size() > 0);

	AABB leftAABB, rightAABB;
	int leftCount = 0;
	int rightCount = 0;

	for (Primitive* primitive : primitives)
	{
		AABB primitiveAABB = primitive->getAABB();

		if (primitiveAABB.center.element(axis) <= divisionPoint)
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
		score += (leftAABB.surfaceArea / node->aabb.surfaceArea) * (double)leftCount;
	}

	if (rightCount > 0)
	{
		rightAABB.update();
		score += (rightAABB.surfaceArea / node->aabb.surfaceArea) * (double)rightCount;
	}

	return score;
}

double BVH::calculateMedianPoint(int axis, const std::vector<Primitive*>& primitives)
{
	assert(primitives.size() >= 2);

	std::vector<double> centerPoints;

	for (Primitive* primitive : primitives)
		centerPoints.push_back(primitive->getAABB().center.element(axis));

	std::sort(centerPoints.begin(), centerPoints.end());
	int size = (int)centerPoints.size();
	double median = 0.0;

	if (size % 2 == 0)
		median = (centerPoints[size / 2 - 1] + centerPoints[size / 2]) / 2.0;
	else
		median = centerPoints[size / 2];

	return median;
}
