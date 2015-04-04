// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Color.h"

namespace Raycer
{
	struct Material
	{
		Color color = Color::WHITE;

		float ambientReflectivity = 1.0f;
		float diffuseReflectivity = 1.0f;
		float specularReflectivity = 1.0f;
		float rayReflectivity = 1.0f;
		float shininess = 1.0f;
	};
}
