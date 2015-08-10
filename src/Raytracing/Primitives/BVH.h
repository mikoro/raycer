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

	class BVH : public Primitive
	{
	public:

		void initialize();
		void intersect(const Ray& ray, Intersection& intersection) const;
		AABB getAABB() const;

		static void construct(const std::vector<Primitive*>& primitives, BVH* node, int maxLeafSize = 5);

	private:

		static void constructRecursive(const std::vector<Primitive*>& primitives, BVH* node, int maxLeafSize, std::mt19937& gen);

		AABB aabb;

		Primitive* left = nullptr;
		Primitive* right = nullptr;
	};
}
