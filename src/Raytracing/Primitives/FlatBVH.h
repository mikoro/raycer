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

	class Ray;
	struct Intersection;

	class FlatBVH : public Primitive
	{
	public:

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection);
		AABB getAABB() const;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate);

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
