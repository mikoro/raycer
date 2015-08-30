// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>
#include <vector>

#include "Raytracing/Primitives/Primitive.h"
#include "Raytracing/Primitives/Triangle.h"
#include "Raytracing/Primitives/FlatBVH.h"
#include "Raytracing/AABB.h"
#include "Math/Vector3.h"
#include "Math/EulerAngle.h"

namespace Raycer
{
	class Ray;
	struct Intersection;

	class Mesh : public Primitive
	{
	public:

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections);
		AABB getAABB() const;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate);

		std::string meshFilePath;
		Vector3 position = Vector3(0.0, 0.0, 0.0);
		Vector3 scale = Vector3(1.0, 1.0, 1.0);
		EulerAngle orientation = EulerAngle(0.0, 0.0, 0.0);
		bool enableBVH = true;
		BVHBuildInfo bvhBuildInfo;

		std::vector<Triangle> triangles;

	private:

		AABB aabb;
		FlatBVH bvh;
	};
}
