// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Shape.h"
#include "Raytracing/Material.h"
#include "Math/Vector3.h"

namespace Raycer
{
	class Triangle : public Shape
	{
	public:

		Triangle();
		
		Material material;
	};
}
