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

		void update();

		Vector3 origin;
		Vector3 direction;

		Vector3 inverseDirection;
		bool dirIsNeg[3];

		double minDistance = 0.0;
		double maxDistance = std::numeric_limits<double>::max();
		double time = 0.0;

		bool fastOcclusion = false;
		bool isShadowRay = false;
		bool isInvalid = false;
		bool isCsgRay = false;
	};
}
