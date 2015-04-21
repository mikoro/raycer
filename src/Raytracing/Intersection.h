// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <limits>

#include "Math/Vector3.h"
#include "Math/Vector2.h"

namespace Raycer
{
	struct Intersection
	{
		bool wasFound = false;
		double distance = std::numeric_limits<double>::max();
		Vector3 position = Vector3(0.0, 0.0, 0.0);
		Vector3 normal = Vector3(0.0, 0.0, 0.0);
		Vector2 texcoord = Vector2(0.0, 0.0);
		int materialId = 0;
	};
}
