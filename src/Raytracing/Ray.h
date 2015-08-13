// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <limits>

#include "Math/Vector3.h"

namespace Raycer
{
	class Ray
	{
	public:

		Ray(const Vector3& origin, const Vector3& direction);

		Vector3 origin;
		Vector3 direction;

		Vector3 inverseDirection;
		bool sign[3];

		double tmin = 0.0;
		double tmax = std::numeric_limits<double>::max();
		double time = 0.0;

		bool fastIntersection = false;
		bool fastOcclusion = false;
	};
}
