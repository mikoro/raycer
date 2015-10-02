// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "cereal/cereal.hpp"

#include "Raytracing/Primitives/Primitive.h"
#include "Math/Vector3.h"
#include "Math/EulerAngle.h"
#include "Math/Matrix4x4.h"

namespace Raycer
{
	class Ray;
	struct Intersection;
	class AABB;

	class Instance : public Primitive
	{
	public:

		friend class Scene;
		friend class CLScene;

		void initialize(const Scene& scene);
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections);
		AABB getAABB() const;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate);

		int primitiveId = 0;

		Vector3 scale = Vector3(1.0, 1.0, 1.0);
		EulerAngle rotate = EulerAngle(0.0, 0.0, 0.0);
		Vector3 translate = Vector3(0.0, 0.0, 0.0);

		bool isTimeVariant = false;
		Vector3 scaleInTime = Vector3(0.0, 0.0, 0.0);
		EulerAngle rotateInTime = EulerAngle(0.0, 0.0, 0.0);
		Vector3 translateInTime = Vector3(0.0, 0.0, 0.0);

		bool changePrimitive = false;

	private:

		bool internalIntersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections, const Matrix4x4& transformation, const Matrix4x4& transformationInv, const Matrix4x4& transformationInvT);

		Primitive* primitive = nullptr;

		Matrix4x4 cachedTransformation;
		Matrix4x4 cachedTransformationInv;
		Matrix4x4 cachedTransformationInvT;

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("primitive", cereal::base_class<Primitive>(this)),
				CEREAL_NVP(primitiveId),
				CEREAL_NVP(scale),
				CEREAL_NVP(rotate),
				CEREAL_NVP(translate),
				CEREAL_NVP(isTimeVariant),
				CEREAL_NVP(scaleInTime),
				CEREAL_NVP(rotateInTime),
				CEREAL_NVP(translateInTime),
				CEREAL_NVP(changePrimitive));
		}
	};
}
