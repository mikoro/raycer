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

	enum class BVHAxisSelection { LARGEST, RANDOM };
	enum class BVHAxisSplit { MIDDLE, MEDIAN, RANDOM };

	struct BVHBuildInfo
	{
		int maxLeafSize = 5;
		bool useSAH = true;
		int regularSAHSplits = 0;
		BVHAxisSelection axisSelection = BVHAxisSelection::LARGEST;
		BVHAxisSplit axisSplit = BVHAxisSplit::MEDIAN;
	};

	struct BVHBuildEntry
	{
		int start;
		int end;
		int parent;
	};

	struct BVHFlatNode
	{
		AABB aabb;
		int rightOffset;
		int startOffset;
		int primitiveCount;
	};

	class BVH : public Primitive
	{
	public:

		void initialize();
		void intersect(const Ray& ray, Intersection& intersection) const;
		AABB getAABB() const;

		void build(const std::vector<Primitive*>& primitives, const BVHBuildInfo& buildInfo);

	private:

		void calculateSplit(int& axis, double& splitPoint, const AABB& nodeAABB, const BVHBuildInfo& buildInfo, const BVHBuildEntry& buildEntry, std::mt19937& generator);
		void calculateSAHSplit(int& axis, double& splitPoint, const AABB& nodeAABB, const BVHBuildInfo& buildInfo, const BVHBuildEntry& buildEntry);
		double calculateSAHScore(int axis, double splitPoint, const AABB& nodeAABB, const BVHBuildEntry& buildEntry);
		double calculateMedianPoint(int axis, const BVHBuildEntry& buildEntry);
		
		std::vector<Primitive*> orderedPrimitives;
		std::vector<BVHFlatNode> flatNodes;
	};
}
