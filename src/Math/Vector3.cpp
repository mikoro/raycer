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

Vector3::Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_)
{
}

Vector3::Vector3(const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

Vector3& Vector3::operator=(const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;

	return *this;
}

Vector3 Raycer::operator+(const Vector3& v, const Vector3& w)
{
	return Vector3(v.x + w.x, v.y + w.y, v.z + w.z);
}

Vector3 Raycer::operator-(const Vector3& v, const Vector3& w)
{
	return Vector3(v.x - w.x, v.y - w.y, v.z - w.z);
}

Vector3 Raycer::operator*(const Vector3& v, float s)
{
	return Vector3(v.x * s, v.y * s, v.z * s);
}

Vector3 Raycer::operator*(float s, const Vector3& v)
{
	return Vector3(v.x * s, v.y * s, v.z * s);
}

Vector3 Raycer::operator/(const Vector3& v, float s)
{
	return Vector3(v.x / s, v.y / s, v.z / s);
}

Vector3 Raycer::operator-(const Vector3& v)
{
	return Vector3(-v.x, -v.y, -v.z);
}

bool Raycer::operator==(const Vector3& v, const Vector3& w)
{
	return MathUtils::almostSame(v.x, w.x) && MathUtils::almostSame(v.y, w.y) && MathUtils::almostSame(v.z, w.z);
}

bool Raycer::operator!=(const Vector3& v, const Vector3& w)
{
	return !(v == w);
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

Vector3 Vector3::reflect(const Vector3& normal) const
{
	return *this - ((2.0f * this->dot(normal)) * normal);
}

Vector3 Vector3::lerp(const Vector3& v1, const Vector3& v2, float t)
{
	assert(t >= 0.0f && t <= 1.0f);

	return v1 * (1.0f - t) + v2 * t;
}
