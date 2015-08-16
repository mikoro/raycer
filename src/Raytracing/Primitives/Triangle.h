// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Primitives/Primitive.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"

namespace Raycer
{
	class Ray;
	struct Intersection;
	class AABB;

	class Triangle : public Primitive
	{
	public:

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection);
		bool intersect2(const Ray& ray, Intersection& intersection);
		AABB getAABB() const;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate);

		Vector3 vertices[3];
		Vector3 normals[3];
		Vector2 texcoords[3];
		Vector3 normal;
	};
}
