// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Math/AxisAngle.h"
#include "Math/Matrix4x4.h"
#include "Math/MathUtils.h"

using namespace Raycer;

AxisAngle::AxisAngle(const Vector3& axis_, double angle_) : axis(axis_), angle(angle_)
{
}

Matrix4x4 AxisAngle::toMatrix4x4() const
{
	double x = axis.x;
	double y = axis.y;
	double z = axis.z;
	double c = cos(MathUtils::degToRad(angle));
	double s = sin(MathUtils::degToRad(angle));
	double ci = 1.0 - c;

	Matrix4x4 result(
		c + x * x * ci, x * y * ci - z * s, x * z * ci + y * s, 0.0,
		y * x * ci + z * s, c + y * y * ci, y * z * ci - x * s, 0.0,
		z * x * ci - y * s, z * y * ci + x * s, c + z * z * ci, 0.0,
		0.0, 0.0, 0.0, 1.0);

	return result;
}
