// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "cereal/cereal.hpp"

#include "Raytracing/Primitives/Primitive.h"
#include "Raytracing/Primitives/FlatBVH.h"

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

		void initialize(const Scene& scene) override;
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections) override;
		AABB getAABB() const override;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate) override;

		std::vector<unsigned> primitiveIds;
		bool enableBVH = true;
		BVHBuildInfo bvhBuildInfo;
		FlatBVH bvh;

	private:

		std::vector<Primitive*> primitives;

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("primitive", cereal::base_class<Primitive>(this)),
				CEREAL_NVP(primitiveIds),
				CEREAL_NVP(enableBVH),
				CEREAL_NVP(bvhBuildInfo),
				CEREAL_NVP(bvh));
		}
	};
}
