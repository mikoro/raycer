// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"

namespace Raycer
{
	class AxisAngle
	{
	public:

		AxisAngle();
		AxisAngle(const Vector3& axis, double angle);
		AxisAngle(const AxisAngle& a);
		AxisAngle& operator=(const AxisAngle& a);

		Vector3 axis;
		double angle; // degrees
	};
}
