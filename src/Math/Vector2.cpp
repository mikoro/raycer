// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>
#include <cassert>

#include "Math/Vector2.h"
#include "Math/MathUtils.h"

using namespace Raycer;

Vector2::Vector2(double x_, double y_) : x(x_), y(y_)
{
}

Vector2::Vector2(const Vector2& v)
{
	x = v.x;
	y = v.y;
}

Vector2& Vector2::operator=(const Vector2& v)
{
	x = v.x;
	y = v.y;

	return *this;
}

namespace Raycer
{
	Vector2 operator+(const Vector2& v, const Vector2& w)
	{
		return Vector2(v.x + w.x, v.y + w.y);
	}

	Vector2 operator-(const Vector2& v, const Vector2& w)
	{
		return Vector2(v.x - w.x, v.y - w.y);
	}

	Vector2 operator*(const Vector2& v, double s)
	{
		return Vector2(v.x * s, v.y * s);
	}

	Vector2 operator*(double s, const Vector2& v)
	{
		return Vector2(v.x * s, v.y * s);
	}

	Vector2 operator/(const Vector2& v, double s)
	{
		return Vector2(v.x / s, v.y / s);
	}

	Vector2 operator-(const Vector2& v)
	{
		return Vector2(-v.x, -v.y);
	}

	bool operator==(const Vector2& v, const Vector2& w)
	{
		return MathUtils::almostSame(v.x, w.x) && MathUtils::almostSame(v.y, w.y);
	}

	bool operator!=(const Vector2& v, const Vector2& w)
	{
		return !(v == w);
	}
}

Vector2& Vector2::operator+=(const Vector2& v)
{
	*this = *this + v;

	return *this;
}

Vector2& Vector2::operator-=(const Vector2& v)
{
	*this = *this - v;

	return *this;
}

Vector2& Vector2::operator*=(double s)
{
	*this = *this * s;

	return *this;
}

Vector2& Vector2::operator/=(double s)
{
	*this = *this / s;

	return *this;
}

double Vector2::length() const
{
	return std::sqrt(x*x + y*y);
}

double Vector2::lengthSquared() const
{
	return (x*x + y*y);
}

void Vector2::normalize()
{
	*this /= length();
}

Vector2 Vector2::normalized() const
{
	return *this / length();
}

bool Vector2::isZero() const
{
	return MathUtils::almostZero(x) && MathUtils::almostZero(y);
}

bool Vector2::isNormal() const
{
	return MathUtils::almostSame(lengthSquared(), 1.0);
}

double Vector2::dot(const Vector2& v) const
{
	return (x * v.x) + (y * v.y);
}

Vector2 Vector2::reflect(const Vector2& normal) const
{
	return *this - ((2.0 * this->dot(normal)) * normal);
}

Vector2 Vector2::lerp(const Vector2& v1, const Vector2& v2, double t)
{
	assert(t >= 0.0 && t <= 1.0);

	return v1 * (1.0 - t) + v2 * t;
}
