// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Color.h"
#include "Math/Vector3.h"

namespace Raycer
{
	struct Pixel
	{
		Color color;
		Vector3 position;
		double distance = 0.0;
	};
}
