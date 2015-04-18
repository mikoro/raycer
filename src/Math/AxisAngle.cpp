// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Math/AxisAngle.h"

using namespace Raycer;

AxisAngle::AxisAngle()
{
}

AxisAngle::AxisAngle(const Vector3& axis_, double angle_)
{
	axis = axis_;
	angle = angle_;
}

AxisAngle::AxisAngle(const AxisAngle& a)
{
	axis = a.axis;
	angle = a.angle;
}

AxisAngle& AxisAngle::operator=(const AxisAngle& a)
{
	axis = a.axis;
	angle = a.angle;

	return *this;
}
