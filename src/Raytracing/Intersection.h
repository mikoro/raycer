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
		double distance = std::numeric_limits<double>::max() / 1024;
		Vector3 position;
		Vector3 normal;
		Vector2 texcoord;
		int materialId = 0;
	};
}
