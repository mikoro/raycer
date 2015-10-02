// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "cereal/cereal.hpp"

#include "Raytracing/Primitives/Primitive.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Ray;
	struct Intersection;
	class AABB;
	class EulerAngle;

	enum class SphereUVMapType { SPHERICAL, LIGHT_PROBE };

	class Sphere : public Primitive
	{
	public:

		friend class Scene;
		friend class CLScene;

		void initialize(const Scene& scene) override;
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections) override;
		AABB getAABB() const override;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate) override;

		Vector3 position;
		double radius = 1.0;
		SphereUVMapType uvMapType = SphereUVMapType::SPHERICAL;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("primitive", cereal::base_class<Primitive>(this)),
				CEREAL_NVP(position),
				CEREAL_NVP(radius),
				CEREAL_NVP(uvMapType));
		}
	};
}
