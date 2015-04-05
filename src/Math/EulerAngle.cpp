// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cassert>

#include "Math/EulerAngle.h"
#include "Math/Vector3.h"
#include "Math/MathUtils.h"

using namespace Raycer;

EulerAngle::EulerAngle(double yaw_, double pitch_, double roll_) : yaw(yaw_), pitch(pitch_), roll(roll_)
{
}

EulerAngle::EulerAngle(const EulerAngle& e)
{
	yaw = e.yaw;
	pitch = e.pitch;
	roll = e.roll;
}

EulerAngle& EulerAngle::operator=(const EulerAngle& e)
{
	yaw = e.yaw;
	pitch = e.pitch;
	roll = e.roll;

	return *this;
}

Vector3 EulerAngle::getDirectionVector() const
{
	Vector3 result;

	result.x = -sin(MathUtils::degToRad(yaw)) * cos(MathUtils::degToRad(pitch));
	result.y = sin(MathUtils::degToRad(pitch));
	result.z = -cos(MathUtils::degToRad(yaw)) * cos(MathUtils::degToRad(pitch));

	return result;
}
