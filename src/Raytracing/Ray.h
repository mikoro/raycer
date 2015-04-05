// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"
#include "Raytracing/Intersection.h"

namespace Raycer
{
	struct Ray
	{
		Ray(const Vector3& origin, const Vector3& direction);
		
		Vector3 origin;
		Vector3 direction;
		Intersection intersection;
	};
}
