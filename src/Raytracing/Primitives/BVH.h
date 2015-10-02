// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <memory>
#include <random>
#include <vector>

#include "Raytracing/Primitives/Primitive.h"

namespace Raycer
{
	class Ray;
	struct Intersection;
	struct BVHBuildInfo;
	class Vector3;
	class EulerAngle;
	class AABB;

	class BVH : public Primitive
	{
	public:

		friend class Scene;
		friend class CLScene;

		void initialize(const Scene& scene) override;
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections) override;
		AABB getAABB() const override;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate) override;

		void build(const std::vector<Primitive*>& primitives, const BVHBuildInfo& info);

	private:

		void buildRecursive(const std::vector<Primitive*>& primitives, BVH* node, const BVHBuildInfo& info, std::mt19937& gen, int& nodeCount, int& leafCount, int previousLeftSize, int previousRightSize);
		void calculateSplit(int& axis, double& splitPoint, const std::vector<Primitive*>& primitives, BVH* node, const BVHBuildInfo& info, std::mt19937& gen);
		void calculateSAHSplit(int& axis, double& splitPoint, const std::vector<Primitive*>& primitives, BVH* node, const BVHBuildInfo& info);
		double calculateSAHScore(int axis, double splitPoint, const std::vector<Primitive*>& primitives, BVH* node);
		double calculateMedianPoint(int axis, const std::vector<Primitive*>& primitives);

		std::unique_ptr<Primitive> left;
		std::unique_ptr<Primitive> right;
	};
}
