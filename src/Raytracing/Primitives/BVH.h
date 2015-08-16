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
	struct BVHBuildInfo;

	class BVH : public Primitive
	{
	public:

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection);
		AABB getAABB() const;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate);

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
