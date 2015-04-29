// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Primitives/Primitive.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Plane : public Primitive
	{
	public:

		void initialize();
		void intersect(Ray& ray) const;

		Vector3 position;
		Vector3 normal;

	private:

		Vector3 uAxis;
		Vector3 vAxis;
	};
}
