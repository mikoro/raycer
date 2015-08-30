// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Raytracing/Primitives/Primitive.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Ray;
	struct Intersection;
	class AABB;
	class EulerAngle;

	enum class CSGOperation { UNION, SUBSTRACTION, INTERSECTION };

	class CSG : public Primitive
	{
	public:

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections);
		AABB getAABB() const;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate);

		CSGOperation operation = CSGOperation::UNION;
		int leftPrimitiveId = 0;
		int rightPrimitiveId = 0;

		Primitive* leftPrimitive;
		Primitive* rightPrimitive;
	};
}
