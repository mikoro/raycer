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

	class Torus : public Primitive
	{
	public:

		friend class Scene;
		friend class CLScene;

		void initialize(const Scene& scene);
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections);
		AABB getAABB() const;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate);

		double outerRadius = 1.0;
		double innerRadius = 0.25;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("primitive", cereal::base_class<Primitive>(this)),
				CEREAL_NVP(outerRadius),
				CEREAL_NVP(innerRadius));
		}
	};
}
