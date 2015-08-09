// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Primitives/Primitive.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Ray;
	struct Intersection;
	class AABB;
	
	class Sphere : public Primitive
	{
	public:

		void initialize();
		void intersect(const Ray& ray, Intersection& intersection) const;
		AABB getAABB() const;

		Vector3 position;
		double radius = 1.0;

	private:

		double radius2;
	};
}
