// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Primitives/Primitive.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Ray;
	struct Intersection;
	class AABB;

	class Box : public Primitive
	{
	public:

		Box();
		Box(const AABB& aabb);

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection) const;
		AABB getAABB() const;

		static Box create(const Vector3& center, const Vector3& extent);

		Vector3 min;
		Vector3 max;
	};
}
