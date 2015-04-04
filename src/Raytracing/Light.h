// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"
#include "Math/Color.h"

namespace Raycer
{
	struct Light
	{
		Vector3 position = Vector3(0, 0, 0);

		Color diffuseColor = Color::WHITE;
		Color specularColor = Color::WHITE;
	};
}
