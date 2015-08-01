// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Primitives/Primitive.h"
#include "Math/Vector3.h"

namespace Raycer
{
	struct Ray;
	
	class Sphere : public Primitive
	{
	public:

		void initialize();
		void intersect(Ray& ray) const;

		Vector3 position;
		double radius = 1.0;
	};
}
