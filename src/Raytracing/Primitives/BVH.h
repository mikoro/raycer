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

	enum class BHVAxisSelection { LARGEST, RANDOM };
	enum class BHVAxisSplit { MIDDLE, MEDIAN, RANDOM };

	struct BVHInfo
	{
		int maxLeafSize = 5;
		BHVAxisSelection axisSelection = BHVAxisSelection::LARGEST;
		BHVAxisSplit axisSplit = BHVAxisSplit::MIDDLE;
		bool useSAH = false;
		int randomSAHSplits = 5;
	};

	class BVH : public Primitive
	{
	public:

		void initialize();
		void intersect(const Ray& ray, Intersection& intersection) const;
		AABB getAABB() const;

		static void construct(const std::vector<Primitive*>& primitives, BVH* node, const BVHInfo& info);

	private:

		static void constructRecursive(const std::vector<Primitive*>& primitives, BVH* node, const BVHInfo& info, std::mt19937& gen);
		static void calculateSplit(int& axis, double& divisionPoint, const std::vector<Primitive*>& primitives, BVH* node, const BVHInfo& info, std::mt19937& gen);
		static void calculateSAHSplit(int& axis, double& divisionPoint, const std::vector<Primitive*>& primitives, BVH* node, const BVHInfo& info, std::mt19937& gen);
		static double calculateSAHScore(int axis, double divisionPoint, const std::vector<Primitive*>& primitives);
		static double calculateMedianPoint(int axis, const std::vector<Primitive*>& primitives);

		AABB aabb;

		Primitive* left = nullptr;
		Primitive* right = nullptr;
	};
}
