// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Raytracing/Primitives/Primitive.h"
#include "Raytracing/Primitives/FlatBVH.h"
#include "Raytracing/AABB.h"

namespace Raycer
{
	class Ray;
	struct Intersection;
	class AABB;
	class Vector3;
	class EulerAngle;

	class PrimitiveGroup : public Primitive
	{
	public:

		friend class Scene;
		friend class CLScene;

		PrimitiveGroup();
		PrimitiveGroup(const std::vector<Primitive*>& primitives);

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections);
		AABB getAABB() const;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate);

		std::vector<int> primitiveIds;
		bool enableBVH = true;
		BVHBuildInfo bvhBuildInfo;

	private:

		std::vector<Primitive*> primitives;

		AABB aabb;
		FlatBVH bvh;
	};
}
