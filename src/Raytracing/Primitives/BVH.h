// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <random>

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

	class BVH : public Primitive
	{
	public:

		void initialize();
		void intersect(const Ray& ray, Intersection& intersection) const;
		AABB getAABB() const;

		void build(const std::vector<Primitive*>& primitives, const BVHBuildInfo& info);
		static void free(BVH* node);

	private:

		void buildRecursive(const std::vector<Primitive*>& primitives, BVH* node, const BVHBuildInfo& info, std::mt19937& gen, int& nodeCount, int& leafCount, int previousLeftSize, int previousRightSize);
		void calculateSplit(int& axis, double& splitPoint, const std::vector<Primitive*>& primitives, BVH* node, const BVHBuildInfo& info, std::mt19937& gen);
		void calculateSAHSplit(int& axis, double& splitPoint, const std::vector<Primitive*>& primitives, BVH* node, const BVHBuildInfo& info);
		double calculateSAHScore(int axis, double splitPoint, const std::vector<Primitive*>& primitives, BVH* node);
		double calculateMedianPoint(int axis, const std::vector<Primitive*>& primitives);

		AABB aabb;

		Primitive* left = nullptr;
		Primitive* right = nullptr;
	};
}
