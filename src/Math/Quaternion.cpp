// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Math/Quaternion.h"
#include "Math/AxisAngle.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"
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
	assert(axis.isNormal());

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

Vector3 Quaternion::rotate(const Vector3& v) const
{
	Vector3 r(x, y, z);
	return v + 2.0 * r.cross(r.cross(v) + w * v);
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

bool Quaternion::isNan() const
{
	return (std::isnan(w) || std::isnan(x) || std::isnan(y) || std::isnan(z));
}

bool Quaternion::isNormal() const
{
	return MathUtils::almostSame(lengthSquared(), 1.0);
}

double Quaternion::dot(const Quaternion& q) const
{
	return (w * q.w) + (x * q.x) + (y * q.y) + (z * q.z);
}

AxisAngle Quaternion::toAxisAngle() const
{
	assert(isNormal());

	AxisAngle aa;

	aa.angle = MathUtils::radToDeg(2.0 * acos(w));
	double sine2 = 1.0 - w * w;

	if (sine2 < std::numeric_limits<double>::epsilon())
	{
		aa.axis.x = 1.0;
		aa.axis.y = 0.0;
		aa.axis.z = 0.0;
	}
	else
	{
		double invSine = 1.0 / sqrt(sine2);

		aa.axis.x = x * invSine;
		aa.axis.y = y * invSine;
		aa.axis.z = z * invSine;
	}

	return aa;
}

Matrix4x4 Quaternion::toMatrix4x4() const
{
	assert(isNormal());

	Matrix4x4 result(
		1.0 - 2.0 * y * y - 2.0 * z * z, 2.0 * x * y - 2.0 * w * z, 2.0 * x * z + 2.0 * w * y, 0.0,
		2.0 * x * y + 2.0 * w * z, 1.0 - 2.0 * x * x - 2.0 * z * z, 2.0 * y * z + 2.0 * w * x, 0.0,
		2.0 * x * z - 2.0 * w * y, 2.0 * y * z - 2.0 * w * x, 1.0 - 2.0 * x * x - 2.0 * y * y, 0.0,
		0.0, 0.0, 0.0, 1.0);

	return result;
}

Quaternion Quaternion::lerp(const Quaternion& q1, const Quaternion& q2, double t)
{
	assert(t >= 0.0 && t <= 1.0);
	return q1 * (1.0 - t) + q2 * t;
}

Quaternion Quaternion::slerp(const Quaternion& q1, const Quaternion& q2, double t)
{
	assert(t >= 0.0 && t <= 1.0);

	double cosine = q1.dot(q2) / (q1.length() * q2.length());
	double theta = acos(cosine);
	double sinTheta = sin(theta);

	if (sinTheta < std::numeric_limits<double>::epsilon())
		return lerp(q1, q2, t);
	
	Quaternion tq1 = (cosine < 0.0) ? -q1 : q1;
	Quaternion q = sin((1.0 - t) * theta) * tq1 + sin(t * theta) * q2;

	return q / sinTheta;
}
