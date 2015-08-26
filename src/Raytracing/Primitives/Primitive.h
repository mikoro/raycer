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
	class EulerAngle;
	struct Material;

	class Primitive
	{
	public:

		virtual ~Primitive() {}

		virtual void initialize() = 0;
		virtual bool intersect(const Ray& ray, Intersection& intersection) = 0;
		virtual AABB getAABB() const = 0;
		virtual void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate) = 0;

		int id = 0;
		int materialId = 0;
		Material* material = nullptr;
	};
}
