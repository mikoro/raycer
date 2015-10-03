// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "cereal/cereal.hpp"

#include "Raytracing/AABB.h"

namespace Raycer
{
	class Scene;
	class Ray;
	struct Intersection;
	class Vector3;
	class EulerAngle;
	class Material;

	class Primitive
	{
	public:

		virtual ~Primitive() {}

		virtual void initialize(const Scene& scene) = 0;
		virtual bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections) = 0;
		virtual AABB getAABB() const = 0;
		virtual void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate) = 0;

		size_t id = 0;
		size_t materialId = 0;
		bool invisible = false;
		Material* material = nullptr;

	protected:

		AABB aabb;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(id),
				CEREAL_NVP(materialId),
				CEREAL_NVP(invisible));
		}
	};
}
