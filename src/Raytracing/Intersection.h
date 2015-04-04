// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"

namespace Raycer
{
	struct Intersection
	{
		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 normal = Vector3(0.0f, 0.0f, 0.0f);
		float tx = 0.0f;
		float ty = 0.0f;
	};
}
