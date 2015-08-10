// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cmath>
#include <limits>

#include "Raytracing/Primitives/BVH.h"
#include "Raytracing/Primitives/PrimitiveList.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Intersection.h"

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

void BVH::construct(const std::vector<Primitive*>& primitives, BVH* node, int maxLeafSize)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	constructRecursive(primitives, node, maxLeafSize, gen);
}

void BVH::constructRecursive(const std::vector<Primitive*>& primitives, BVH* node, int maxLeafSize, std::mt19937& gen)
{
	for (Primitive* primitive : primitives)
		node->aabb.expand(primitive->getAABB());

	node->aabb.update();

	std::uniform_int_distribution<int> dist(0, 2);
	//int axis = node->aabb.getLargestAxis();
	int axis = dist(gen);
	double divisionPoint = node->aabb.center.element(axis);

	std::vector<Primitive*> leftPrimitives;
	std::vector<Primitive*> rightPrimitives;

	for (Primitive* primitive : primitives)
	{
		if (primitive->getAABB().center.element(axis) <= divisionPoint)
			leftPrimitives.push_back(primitive);
		else
			rightPrimitives.push_back(primitive);
	}

	if (leftPrimitives.size() > maxLeafSize)
	{
		node->left = new BVH();
		constructRecursive(leftPrimitives, (BVH*)node->left, maxLeafSize, gen);
	}
	else
		node->left = new PrimitiveList(leftPrimitives);

	if (rightPrimitives.size() > maxLeafSize)
	{
		node->right = new BVH();
		constructRecursive(rightPrimitives, (BVH*)node->right, maxLeafSize, gen);
	}
	else
		node->right = new PrimitiveList(rightPrimitives);
}
