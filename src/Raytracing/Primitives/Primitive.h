// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Math/Vector2.h"

namespace Raycer
{
	class Ray;
	struct Intersection;
	class AABB;
	class Vector3;

	class Primitive
	{
	public:

		virtual ~Primitive() {}

		virtual void initialize() = 0;
		virtual bool intersect(const Ray& ray, Intersection& intersection) = 0;
		virtual AABB getAABB() const = 0;
		virtual Vector3* getPosition() = 0;

		int materialId = 0;
		Vector2 texcoordScale = Vector2(1.0, 1.0);
	};
}
