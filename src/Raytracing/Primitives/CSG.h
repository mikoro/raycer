// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "cereal/cereal.hpp"

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

		void initialize(const Scene& scene) override;
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections) override;
		AABB getAABB() const override;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate) override;

		CSGOperation operation = CSGOperation::UNION;
		unsigned leftPrimitiveId = 0;
		unsigned rightPrimitiveId = 0;

	private:

		Primitive* leftPrimitive = nullptr;
		Primitive* rightPrimitive = nullptr;

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("primitive", cereal::base_class<Primitive>(this)),
				CEREAL_NVP(operation),
				CEREAL_NVP(leftPrimitiveId),
				CEREAL_NVP(rightPrimitiveId));
		}
	};
}
