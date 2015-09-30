// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Math/EulerAngle.h"
#include "Math/Vector3.h"
#include "Math/MathUtils.h"

using namespace Raycer;

EulerAngle::EulerAngle(double pitch_, double yaw_, double roll_) : pitch(pitch_), yaw(yaw_), roll(roll_)
{
}

EulerAngle::EulerAngle(const EulerAngle& e)
{
	pitch = e.pitch;
	yaw = e.yaw;
	roll = e.roll;
}

EulerAngle& EulerAngle::operator=(const EulerAngle& e)
{
	pitch = e.pitch;
	yaw = e.yaw;
	roll = e.roll;

	return *this;
}

namespace Raycer
{
	EulerAngle operator+(const EulerAngle& e1, const EulerAngle& e2)
	{
		return EulerAngle(e1.pitch + e2.pitch, e1.yaw + e2.yaw, e1.roll + e2.roll);
	}

	EulerAngle operator-(const EulerAngle& e1, const EulerAngle& e2)
	{
		return EulerAngle(e1.pitch - e2.pitch, e1.yaw - e2.yaw, e1.roll - e2.roll);
	}

	EulerAngle operator*(const EulerAngle& e, double s)
	{
		return EulerAngle(e.pitch * s, e.yaw * s, e.roll * s);
	}

	EulerAngle operator*(double s, const EulerAngle& e)
	{
		return EulerAngle(e.pitch * s, e.yaw * s, e.roll * s);
	}

	EulerAngle operator-(const EulerAngle& e)
	{
		return EulerAngle(-e.pitch, -e.yaw, -e.roll);
	}
}

EulerAngle& EulerAngle::operator+=(const EulerAngle& e)
{
	*this = *this + e;

	return *this;
}

EulerAngle& EulerAngle::operator-=(const EulerAngle& e)
{
	*this = *this - e;

	return *this;
}

void EulerAngle::clampPitch()
{
	if (pitch > 89.0)
		pitch = 89.0;

	if (pitch < -89.0)
		pitch = -89.0;
}

void EulerAngle::normalize()
{
	while (std::abs(pitch) > 180.0)
		pitch += (pitch > 0.0) ? -360.0 : 360.0;

	while (std::abs(yaw) > 180.0)
		yaw += (yaw > 0.0) ? -360.0 : 360.0;

	while (std::abs(roll) > 180.0)
		roll += (roll > 0.0) ? -360.0 : 360.0;
}

Vector3 EulerAngle::getDirection() const
{
	Vector3 result;

	result.x = -sin(MathUtils::degToRad(yaw)) * cos(MathUtils::degToRad(pitch));
	result.y = sin(MathUtils::degToRad(pitch));
	result.z = -cos(MathUtils::degToRad(yaw)) * cos(MathUtils::degToRad(pitch));

	return result.normalized();
}

EulerAngle EulerAngle::lerp(const EulerAngle& e1, const EulerAngle& e2, double t)
{
	EulerAngle result;
	double oneMinusT = 1.0 - t;

	result.pitch = e1.pitch * oneMinusT + e2.pitch * t;
	result.yaw = e1.yaw * oneMinusT + e2.yaw * t;
	result.roll = e1.roll * oneMinusT + e2.roll * t;

	return result;
}
