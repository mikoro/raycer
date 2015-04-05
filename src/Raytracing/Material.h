// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Color.h"

namespace Raycer
{
	struct Material
	{
		Color color = Color::WHITE;

		double diffuseReflectivity = 1.0;
		double specularReflectivity = 1.0;
		double rayReflectivity = 1.0;
		double shininess = 1.0;
	};
}
