// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Raytracing/Primitives/Primitive.h"
#include "Math/Vector3.h"

#undef IN
#undef OUT
#undef DIFFERENCE

namespace Raycer
{
	class Ray;
	class AABB;
	class EulerAngle;

	enum class CSGSide { LEFT, RIGHT };
	enum class CSGDirection { IN, OUT };
	enum class CSGOperation { UNION, DIFFERENCE, INTERSECTION };

	class CSG : public Primitive
	{
	public:

		friend class Scene;
		friend class CLScene;

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections);
		AABB getAABB() const;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate);

		CSGOperation operation = CSGOperation::UNION;
		int leftPrimitiveId = 0;
		int rightPrimitiveId = 0;

	private:

		Primitive* leftPrimitive = nullptr;
		Primitive* rightPrimitive = nullptr;
	};
}
