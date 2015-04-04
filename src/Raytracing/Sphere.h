// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Shape.h"
#include "Raytracing/Material.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Sphere : public Shape
	{
	public:

		Sphere();
		Sphere(const Vector3& position, float radius, const Material& material);

		Vector3 position = Vector3(0, 0, 0);
		float radius = 1.0f;
		Material material;
	};
}
