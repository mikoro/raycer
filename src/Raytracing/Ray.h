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

		void precalculate();

		Vector3 origin;
		Vector3 direction;
		Vector3 inverseDirection;

		double minDistance = std::numeric_limits<double>::lowest();
		double maxDistance = std::numeric_limits<double>::max();

		double time = 0.0;

		bool isShadowRay = false;
		bool fastOcclusion = false;
	};
}
