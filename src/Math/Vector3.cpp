// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>
#include <cassert>

#include "Math/Vector3.h"
#include "Math/MathUtils.h"

using namespace Raycer;

const Vector3 Vector3::RIGHT = Vector3(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UP = Vector3(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::FORWARD = Vector3(0.0f, 0.0f, 1.0f);

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z)
{
}

Vector3 Vector3::operator+(const Vector3& v) const
{
	return Vector3(x + v.x, y + v.y, z + v.z);
}

Vector3 Vector3::operator-(const Vector3& v) const
{
	return Vector3(x - v.x, y - v.y, z - v.z);
}

Vector3 Vector3::operator*(float s) const
{
	return Vector3(x * s, y * s, z * s);
}

Vector3 Vector3::operator/(float s) const
{
	return Vector3(x / s, y / s, z / s);
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

Vector3& Vector3::operator+=(const Vector3& v)
{
	*this = *this + v;

	return *this;
}

Vector3& Vector3::operator-=(const Vector3& v)
{
	*this = *this - v;

	return *this;
}

Vector3& Vector3::operator*=(float s)
{
	*this = *this * s;

	return *this;
}

Vector3& Vector3::operator/=(float s)
{
	*this = *this / s;

	return *this;
}

bool Vector3::operator==(const Vector3& v) const
{
	return MathUtils::almostSame(x, v.x) && MathUtils::almostSame(y, v.y) && MathUtils::almostSame(z, v.z);
}

bool Vector3::operator!=(const Vector3& v) const
{
	return !(*this == v);
}

float Vector3::length() const
{
	return std::sqrt(x*x + y*y + z*z);
}

float Vector3::lengthSquared() const
{
	return (x*x + y*y + z*z);
}

void Vector3::normalize()
{
	*this /= length();
}

Vector3 Vector3::normalized() const
{
	return *this / length();
}

bool Vector3::isZero() const
{
	return MathUtils::almostZero(x) && MathUtils::almostZero(y) && MathUtils::almostZero(z);
}

bool Vector3::isNormal() const
{
	return MathUtils::almostSame(lengthSquared(), 1.0f);
}

float Vector3::dot(const Vector3& v) const
{
	return (x * v.x) + (y * v.y) + (z * v.z);
}

Vector3 Vector3::cross(const Vector3& v) const
{
	Vector3 r;

	r.x = y * v.z - z * v.y;
	r.y = z * v.x - x * v.z;
	r.z = x * v.y - y * v.x;

	return r;
}

Vector3 Vector3::lerp(const Vector3& v1, const Vector3& v2, float t)
{
	assert(t >= 0.0f && t <= 1.0f);

	return v1 * (1.0f - t) + v2 * t;
}
