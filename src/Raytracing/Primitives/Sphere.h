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
	class EulerAngle;

	class Sphere : public Primitive
	{
	public:

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection);
		AABB getAABB() const;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate);

		Vector3 position;
		Vector3 displacement;
		double radius = 1.0;
	};
}
