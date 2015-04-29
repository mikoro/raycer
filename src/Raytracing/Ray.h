// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Raytracing/Intersection.h"

namespace Raycer
{
	struct Ray
	{
		Ray() {}

		Ray(const Vector3& origin_, const Vector3& direction_, int reflectionCount_)
		{
			origin = origin_;
			direction = direction_;
			reflectionCount = reflectionCount_;
		}
		
		Vector3 origin;
		Vector3 direction;
		Intersection intersection;
		Color color;
		int reflectionCount = 0;
	};
}
