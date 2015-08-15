// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

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

		void initialize();
		bool intersect(const Ray& ray, Intersection& intersection);
		AABB getAABB() const;
		Vector3* getPosition();

		EulerAngle rotate = EulerAngle(0.0, 0.0, 0.0);
		Vector3 scale = Vector3(1.0, 1.0, 1.0);
		Vector3 translate = Vector3(0.0, 0.0, 0.0);
		
		Primitive* primitive = nullptr;

	private:

		Matrix4x4 transformation;
		Matrix4x4 transformationInv;
		Matrix4x4 transformationInvT;
	};
}
