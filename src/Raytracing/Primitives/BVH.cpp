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
	int stack[128];
	int stackptr = 0;

	// push to stack
	stack[stackptr] = 0;
	stackptr++;

	while (stackptr > 0)
	{
		// pop from stack
		stackptr--;
		int index = stack[stackptr];
		BVHFlatNode flatNode = flatNodes[index];

		// leaf node -> intersect with all its primitives
		if (flatNode.rightOffset == 0)
		{
			for (int i = 0; i < flatNode.primitiveCount; ++i)
				orderedPrimitives[flatNode.startOffset + i]->intersect(ray, intersection);
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
}

AABB BVH::getAABB() const
{
	return flatNodes[0].aabb;
}

void BVH::build(const std::vector<Primitive*>& primitives, const BVHBuildInfo& buildInfo)
{
	Log& log = App::getLog();

	log.logInfo("Building BVH (primitives: %d)", primitives.size());
	
	auto startTime = std::chrono::high_resolution_clock::now();

	std::random_device rd;
	std::mt19937 generator(rd());

	BVHBuildEntry stack[128];
	orderedPrimitives = primitives;
	flatNodes.clear();

	int stackptr = 0;
	int nodeCount = 0;
	int leafCount = 0;

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
		BVHFlatNode flatNode;
		BVHBuildEntry buildEntry = stack[stackptr];
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

		int axis = flatNode.aabb.getLargestAxis();
		double splitPoint = flatNode.aabb.center.element(axis);
		int middle = buildEntry.start;

		// partition primitive range by the split point
		for (int i = buildEntry.start; i < buildEntry.end; ++i)
		{
			if (orderedPrimitives[i]->getAABB().center.element(axis) < splitPoint)
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
