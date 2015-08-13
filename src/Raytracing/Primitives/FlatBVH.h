// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <random>
#include <vector>

#include "Raytracing/Primitives/Primitive.h"
#include "Raytracing/AABB.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Ray;
	struct Intersection;
	struct BVHBuildInfo;

	struct FlatBVHBuildEntry
	{
		int start;
		int end;
		int parent;
	};

	struct FlatBVHNode
	{
		AABB aabb;
		int rightOffset;
		int startOffset;
		int primitiveCount;
	};

	class FlatBVH : public Primitive
	{
	public:

		void initialize();
		void intersect(const Ray& ray, Intersection& intersection) const;
		AABB getAABB() const;

		void build(const std::vector<Primitive*>& primitives, const BVHBuildInfo& buildInfo);

	private:

		void calculateSplit(int& axis, double& splitPoint, const AABB& nodeAABB, const BVHBuildInfo& buildInfo, const FlatBVHBuildEntry& buildEntry, std::mt19937& generator);
		void calculateSAHSplit(int& axis, double& splitPoint, const AABB& nodeAABB, const BVHBuildInfo& buildInfo, const FlatBVHBuildEntry& buildEntry);
		double calculateSAHScore(int axis, double splitPoint, const AABB& nodeAABB, const FlatBVHBuildEntry& buildEntry);
		double calculateMedianPoint(int axis, const FlatBVHBuildEntry& buildEntry);
		
		std::vector<Primitive*> orderedPrimitives;
		std::vector<FlatBVHNode> flatNodes;
	};
}
