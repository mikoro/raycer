// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>
#include <cassert>
#include <stdexcept>

#include "tinyformat/tinyformat.h"

#include "Math/Vector3.h"
#include "Math/MathUtils.h"

using namespace Raycer;

const Vector3 Vector3::RIGHT = Vector3(1.0, 0.0, 0.0);
const Vector3 Vector3::UP = Vector3(0.0, 1.0, 0.0);
const Vector3 Vector3::FORWARD = Vector3(0.0, 0.0, 1.0);

Vector3::Vector3(double x_, double y_, double z_) : x(x_), y(y_), z(z_)
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

namespace Raycer
{
	Vector3 operator+(const Vector3& v, const Vector3& w)
	{
		return Vector3(v.x + w.x, v.y + w.y, v.z + w.z);
	}

	Vector3 operator-(const Vector3& v, const Vector3& w)
	{
		return Vector3(v.x - w.x, v.y - w.y, v.z - w.z);
	}

	Vector3 operator*(const Vector3& v, const Vector3& w)
	{
		return Vector3(v.x * w.x, v.y * w.y, v.z * w.z);
	}

	Vector3 operator*(const Vector3& v, double s)
	{
		return Vector3(v.x * s, v.y * s, v.z * s);
	}

	Vector3 operator*(double s, const Vector3& v)
	{
		return Vector3(v.x * s, v.y * s, v.z * s);
	}

	Vector3 operator/(const Vector3& v, const Vector3& w)
	{
		return Vector3(v.x / w.x, v.y / w.y, v.z / w.z);
	}

	Vector3 operator/(const Vector3& v, double s)
	{
		return Vector3(v.x / s, v.y / s, v.z / s);
	}

	Vector3 operator-(const Vector3& v)
	{
		return Vector3(-v.x, -v.y, -v.z);
	}

	bool operator==(const Vector3& v, const Vector3& w)
	{
		return MathUtils::almostSame(v.x, w.x) && MathUtils::almostSame(v.y, w.y) && MathUtils::almostSame(v.z, w.z);
	}

	bool operator!=(const Vector3& v, const Vector3& w)
	{
		return !(v == w);
	}

	bool operator>(const Vector3& v, const Vector3& w)
	{
		return v.x > w.x && v.y > w.y && v.z > w.z;
	}

	bool operator<(const Vector3& v, const Vector3& w)
	{
		return v.x < w.x && v.y < w.y && v.z < w.z;
	}
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

Vector3& Vector3::operator*=(const Vector3& v)
{
	*this = *this * v;

	return *this;
}

Vector3& Vector3::operator*=(double s)
{
	*this = *this * s;

	return *this;
}

Vector3& Vector3::operator/=(const Vector3& v)
{
	*this = *this / v;

	return *this;
}

Vector3& Vector3::operator/=(double s)
{
	*this = *this / s;

	return *this;
}

double Vector3::length() const
{
	return std::sqrt(x*x + y*y + z*z);
}

double Vector3::lengthSquared() const
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

void Vector3::inverse()
{
	x = 1.0 / x;
	y = 1.0 / y;
	z = 1.0 / z;
}

Vector3 Vector3::inversed() const
{
	Vector3 inverse;
	inverse.x = 1.0 / x;
	inverse.y = 1.0 / y;
	inverse.z = 1.0 / z;

	return inverse;
}

bool Vector3::isZero() const
{
	return (x == 0.0 && y == 0.0 && z == 0.0);
}

bool Vector3::isNormal() const
{
	return MathUtils::almostSame(lengthSquared(), 1.0);
}

double Vector3::dot(const Vector3& v) const
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
	return *this - ((2.0 * this->dot(normal)) * normal);
}

std::string Vector3::toString() const
{
	return tfm::format("(%.2f, %.2f, %.2f)", x, y, z);
}

double Vector3::element(int number) const
{
	switch (number)
	{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: throw std::runtime_error("Invalid vector element number");
	}
}

Vector3 Vector3::lerp(const Vector3& v1, const Vector3& v2, double t)
{
	assert(t >= 0.0 && t <= 1.0);

	return v1 * (1.0 - t) + v2 * t;
}

Vector3 Vector3::abs(const Vector3& v)
{
	return Vector3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}
