// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Primitives/BVH.h"
#include "Raytracing/Primitives/FlatBVH.h"
#include "Raytracing/Primitives/PrimitiveGroup.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/AABB.h"
#include "App.h"
#include "Utils/Log.h"
#include "Math/Vector3.h"
#include "Math/EulerAngle.h"

using namespace Raycer;

void Raycer::BVH::initialize(const Scene& scene)
{
	(void)scene;
}

bool BVH::intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections)
{
	if (ray.fastOcclusion && intersection.wasFound)
		return true;

	if (!aabb.intersects(ray))
		return false;

	bool wasFound = false;

	if (left->intersect(ray, intersection, intersections))
	{
		if (ray.fastOcclusion)
			return true;

		wasFound = true;
	}

	if (right->intersect(ray, intersection, intersections))
	{
		if (ray.fastOcclusion)
			return true;

		wasFound = true;
	}

	return wasFound;
}

AABB BVH::getAABB() const
{
	return aabb;
}

void BVH::transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate)
{
	(void)scale;
	(void)rotate;
	(void)translate;
}

void BVH::build(const std::vector<Primitive*>& primitives, const BVHBuildInfo& info)
{
	Log& log = App::getLog();

	log.logInfo("Building BVH (primitives: %d)", primitives.size());

	auto startTime = std::chrono::high_resolution_clock::now();
	std::random_device rd;
	std::mt19937 gen(rd());
	int nodeCount = 0;
	int leafCount = 0;

	buildRecursive(primitives, this, info, gen, nodeCount, leafCount, 0, 0);

	auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
	int milliseconds = (int)std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();

	log.logInfo("BVH building finished (time: %d ms, nodes: %d, leafs: %d)", milliseconds, nodeCount, leafCount);
}

void BVH::buildRecursive(const std::vector<Primitive*>& primitives, BVH* node, const BVHBuildInfo& info, std::mt19937& gen, int& nodeCount, int& leafCount, int previousLeftSize, int previousRightSize)
{
	nodeCount++;

	for (Primitive* primitive : primitives)
		node->aabb.expand(primitive->getAABB());

	int axis;
	double splitPoint;

	calculateSplit(axis, splitPoint, primitives, node, info, gen);

	std::vector<Primitive*> leftPrimitives;
	std::vector<Primitive*> rightPrimitives;

	for (Primitive* primitive : primitives)
	{
		if (primitive->getAABB().getCenter().element(axis) <= splitPoint)
			leftPrimitives.push_back(primitive);
		else
			rightPrimitives.push_back(primitive);
	}

	int leftSize = (int)leftPrimitives.size();
	int rightSize = (int)rightPrimitives.size();
	bool shouldTerminate = false;

	if (leftSize == previousLeftSize && rightSize == previousRightSize)
		shouldTerminate = true;

	previousLeftSize = leftSize;
	previousRightSize = rightSize;

	if (leftPrimitives.size() > (size_t)info.maxLeafSize && !shouldTerminate)
	{
		node->left = std::unique_ptr<BVH>(new BVH());
		buildRecursive(leftPrimitives, (BVH*)node->left.get(), info, gen, nodeCount, leafCount, previousLeftSize, previousRightSize);
	}
	else
	{
		leafCount++;
		node->left = std::unique_ptr<PrimitiveGroup>(new PrimitiveGroup(leftPrimitives));
	}

	if (rightPrimitives.size() > (size_t)info.maxLeafSize && !shouldTerminate)
	{
		node->right = std::unique_ptr<BVH>(new BVH());
		buildRecursive(rightPrimitives, (BVH*)node->right.get(), info, gen, nodeCount, leafCount, previousLeftSize, previousRightSize);
	}
	else
	{
		leafCount++;
		node->right = std::unique_ptr<PrimitiveGroup>(new PrimitiveGroup(rightPrimitives));
	}
}

void BVH::calculateSplit(int& axis, double& splitPoint, const std::vector<Primitive*>& primitives, BVH* node, const BVHBuildInfo& info, std::mt19937& gen)
{
	if (info.useSAH)
	{
		calculateSAHSplit(axis, splitPoint, primitives, node, info);
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
		splitPoint = node->aabb.getCenter().element(axis);
	else if (info.axisSplit == BVHAxisSplit::MEDIAN)
		splitPoint = calculateMedianPoint(axis, primitives);
	else if (info.axisSplit == BVHAxisSplit::RANDOM)
	{
		std::uniform_real_distribution<double> dist(node->aabb.getMin().element(axis), node->aabb.getMax().element(axis));
		splitPoint = dist(gen);
	}
	else
		throw std::runtime_error("Unknown BVH axis split");
}

void BVH::calculateSAHSplit(int& axis, double& splitPoint, const std::vector<Primitive*>& primitives, BVH* node, const BVHBuildInfo& info)
{
	double lowestScore = std::numeric_limits<double>::max();

	for (int tempAxis = 0; tempAxis <= 2; ++tempAxis)
	{
		double tempSplitPoint = node->aabb.getCenter().element(tempAxis);
		double score = calculateSAHScore(tempAxis, tempSplitPoint, primitives, node);

		if (score < lowestScore)
		{
			axis = tempAxis;
			splitPoint = tempSplitPoint;
			lowestScore = score;
		}

		tempSplitPoint = calculateMedianPoint(tempAxis, primitives);
		score = calculateSAHScore(tempAxis, tempSplitPoint, primitives, node);

		if (score < lowestScore)
		{
			axis = tempAxis;
			splitPoint = tempSplitPoint;
			lowestScore = score;
		}

		if (info.regularSAHSplits > 0)
		{
			double step = node->aabb.getExtent().element(tempAxis) / (double)info.regularSAHSplits;
			tempSplitPoint = node->aabb.getMin().element(tempAxis);

			for (int i = 0; i < info.regularSAHSplits - 1; ++i)
			{
				tempSplitPoint += step;
				score = calculateSAHScore(tempAxis, tempSplitPoint, primitives, node);

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

double BVH::calculateSAHScore(int axis, double splitPoint, const std::vector<Primitive*>& primitives, BVH* node)
{
	assert(primitives.size() > 0);

	AABB leftAABB, rightAABB;
	int leftCount = 0;
	int rightCount = 0;

	for (Primitive* primitive : primitives)
	{
		AABB primitiveAABB = primitive->getAABB();

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
		score += (leftAABB.getSurfaceArea() / node->aabb.getSurfaceArea()) * (double)leftCount;

	if (rightCount > 0)
		score += (rightAABB.getSurfaceArea() / node->aabb.getSurfaceArea()) * (double)rightCount;

	return score;
}

double BVH::calculateMedianPoint(int axis, const std::vector<Primitive*>& primitives)
{
	assert(primitives.size() >= 2);

	std::vector<double> centerPoints;

	for (Primitive* primitive : primitives)
		centerPoints.push_back(primitive->getAABB().getCenter().element(axis));

	std::sort(centerPoints.begin(), centerPoints.end());
	int size = (int)centerPoints.size();
	double median = 0.0;

	if (size % 2 == 0)
		median = (centerPoints[size / 2 - 1] + centerPoints[size / 2]) / 2.0;
	else
		median = centerPoints[size / 2];

	return median;
}
