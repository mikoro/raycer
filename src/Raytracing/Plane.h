// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Shape.h"
#include "Raytracing/Material.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Plane : public Shape
	{
	public:

		Plane();
		
		Vector3 position = Vector3(0, 0, 0);
		Material material;
	};
}
