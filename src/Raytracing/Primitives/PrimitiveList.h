// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Raytracing/Primitives/Primitive.h"

namespace Raycer
{
	class Ray;
	struct Intersection;
	class AABB;

	class PrimitiveList : public Primitive
	{
	public:

		PrimitiveList();
		PrimitiveList(const std::vector<Primitive*>& primitives);

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection);
		AABB getAABB() const;
		Vector3* getPosition();

		std::vector<Primitive*> primitives;
	};
}
