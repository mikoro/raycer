// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Math/Quaternion.h"
#include "Math/Vector3.h"
#include "Math/AxisAngle.h"
#include "Math/MathUtils.h"

using namespace Raycer;

Quaternion::Quaternion(double w_, double x_, double y_, double z_) : w(w_), x(x_), y(y_), z(z_)
{
}

Quaternion::Quaternion(const AxisAngle& axisAngle) : Quaternion(axisAngle.axis, axisAngle.angle)
{
}

Quaternion::Quaternion(const Vector3& axis, double angle)
{
	double radiansPer2 = MathUtils::degToRad(angle) / 2.0;
	double cosine = cos(radiansPer2);
	double sine = sin(radiansPer2);

	w = cosine;
	x = axis.x * sine;
	y = axis.y * sine;
	z = axis.z * sine;
}

namespace Raycer
{
	Quaternion operator+(const Quaternion& q1, const Quaternion& q2)
	{
		return Quaternion(q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z);
	}

	Quaternion operator-(const Quaternion& q1, const Quaternion& q2)
	{
		return Quaternion(q1.w - q2.w, q1.x - q2.x, q1.y - q2.y, q1.z - q2.z);
	}

	Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
	{
		Quaternion r;

		r.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
		r.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
		r.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
		r.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;

		return r;
	}

	Vector3 operator*(const Quaternion& q, const Vector3& v)
	{
		Quaternion p(0.0, v.x, v.y, v.z);
		Quaternion r = q * p * q.conjugated();

		return Vector3(r.x, r.y, r.z);
	}

	Vector3 operator*(const Vector3& v, const Quaternion& q)
	{
		return q * v;
	}

	Quaternion operator*(const Quaternion& q, double s)
	{
		return Quaternion(q.w * s, q.x * s, q.y * s, q.z * s);
	}

	Quaternion operator*(double s, const Quaternion& q)
	{
		return q * s;
	}

	Quaternion operator/(const Quaternion& q, double s)
	{
		return Quaternion(q.w / s, q.x / s, q.y / s, q.z / s);
	}

	Quaternion operator-(const Quaternion& q)
	{
		return Quaternion(-q.w, -q.x, -q.y, -q.z);
	}

	bool operator==(const Quaternion& q1, const Quaternion& q2)
	{
		return MathUtils::almostSame(q1.w, q2.w) && MathUtils::almostSame(q1.x, q2.x) && MathUtils::almostSame(q1.y, q2.y) && MathUtils::almostSame(q1.z, q2.z);
	}

	bool operator!=(const Quaternion& q1, const Quaternion& q2)
	{
		return !(q1 == q2);
	}
}

Quaternion& Quaternion::operator+=(const Quaternion& q)
{
	*this = *this + q;
	return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& q)
{
	*this = *this - q;
	return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& q)
{
	*this = *this * q;
	return *this;
}

Quaternion& Quaternion::operator*=(double s)
{
	*this = *this * s;
	return *this;
}

Quaternion& Quaternion::operator/=(double s)
{
	*this = *this / s;
	return *this;
}

double Quaternion::length() const
{
	return std::sqrt(w * w + x * x + y * y + z * z);
}

double Quaternion::lengthSquared() const
{
	return (w * w + x * x + y * y + z * z);
}

void Quaternion::conjugate()
{
	x = -x;
	y = -y;
	z = -z;
}

Quaternion Quaternion::conjugated() const
{
	return Quaternion(w, -x, -y, -z);
}

void Quaternion::normalize()
{
	*this /= length();
}

Quaternion Quaternion::normalized() const
{
	return *this / length();
}

bool Quaternion::isZero() const
{
	return (w == 0.0 && x == 0.0 && y == 0.0 && z == 0.0);
}

bool Quaternion::isNormal() const
{
	return MathUtils::almostSame(lengthSquared(), 1.0);
}

AxisAngle Quaternion::toAxisAngle() const
{
	AxisAngle a;

	double cosine = w;
	double sine = 1.0 - cosine * cosine;

	a.angle = MathUtils::radToDeg(acos(cosine) * 2.0);
	a.axis.x = x / sine;
	a.axis.y = y / sine;
	a.axis.z = z / sine;

	return a;
}

Quaternion Quaternion::slerp(const Quaternion& q1, const Quaternion& q2, double t)
{
	(void)q1;
	(void)q2;
	(void)t;

	return Quaternion();
}
