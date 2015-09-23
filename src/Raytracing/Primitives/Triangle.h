// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Raytracing/Primitives/Primitive.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"

namespace Raycer
{
	class Ray;
	struct Intersection;
	class AABB;
	class EulerAngle;

	class Triangle : public Primitive
	{
	public:

		friend class Scene;
		friend class CLScene;

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections);
		AABB getAABB() const;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate);

		Vector3 vertices[3];
		Vector3 normals[3];
		Vector2 texcoords[3];
		Vector3 normal;
		Vector3 tangent;
		Vector3 bitangent;
	};
}
