// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Math/Matrix4x4.h"
#include "Math/MathUtils.h"
#include "Math/Vector3.h"
#include "Math/EulerAngle.h"

using namespace Raycer;

const Matrix4x4 Matrix4x4::IDENTITY = Matrix4x4(
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0);

const Matrix4x4 Matrix4x4::ZERO = Matrix4x4();

Matrix4x4::Matrix4x4()
{
	std::memset(m, 0, sizeof(double) * 16);
}

Matrix4x4::Matrix4x4(const Matrix4x4& n)
{
	std::memcpy(m, n.m, sizeof(double) * 16);
}

Matrix4x4::Matrix4x4(double m00, double m10, double m20, double m30, double m01, double m11, double m21, double m31, double m02, double m12, double m22, double m32, double m03, double m13, double m23, double m33)
{
	m[0][0] = m00; m[1][0] = m10; m[2][0] = m20; m[3][0] = m30;
	m[0][1] = m01; m[1][1] = m11; m[2][1] = m21; m[3][1] = m31;
	m[0][2] = m02; m[1][2] = m12; m[2][2] = m22; m[3][2] = m32;
	m[0][3] = m03; m[1][3] = m13; m[2][3] = m23; m[3][3] = m33;
}

Matrix4x4::Matrix4x4(const Vector3& r, const Vector3& u, const Vector3& f)
{
	m[0][0] = r.x; m[1][0] = u.x; m[2][0] = f.x; m[3][0] = 0.0;
	m[0][1] = r.y; m[1][1] = u.y; m[2][1] = f.y; m[3][1] = 0.0;
	m[0][2] = r.z; m[1][2] = u.z; m[2][2] = f.z; m[3][2] = 0.0;
	m[0][3] = 0.0; m[1][3] = 0.0; m[2][3] = 0.0; m[3][3] = 1.0;
}

Matrix4x4& Matrix4x4::operator=(const Matrix4x4& n)
{
	std::memcpy(m, n.m, sizeof(double) * 16);

	return *this;
}

namespace Raycer
{
	Matrix4x4 operator+(const Matrix4x4& m, const Matrix4x4& n)
	{
		Matrix4x4 result;

		for (int c = 0; c < 4; ++c)
			for (int r = 0; r < 4; ++r)
				result.m[c][r] = m.m[c][r] + n.m[c][r];

		return result;
	}

	Matrix4x4 operator-(const Matrix4x4& m, const Matrix4x4& n)
	{
		Matrix4x4 result;

		for (int c = 0; c < 4; ++c)
			for (int r = 0; r < 4; ++r)
				result.m[c][r] = m.m[c][r] - n.m[c][r];

		return result;
	}

	Matrix4x4 operator*(const Matrix4x4& m, double s)
	{
		Matrix4x4 result;

		for (int c = 0; c < 4; ++c)
			for (int r = 0; r < 4; ++r)
				result.m[c][r] = m.m[c][r] * s;

		return result;
	}

	Matrix4x4 operator*(double s, const Matrix4x4& m)
	{
		Matrix4x4 result;

		for (int c = 0; c < 4; ++c)
			for (int r = 0; r < 4; ++r)
				result.m[c][r] = m.m[c][r] * s;

		return result;
	}

	Matrix4x4 operator*(const Matrix4x4& m, const Matrix4x4& n)
	{
		Matrix4x4 result;

		for (int c = 0; c < 4; ++c)
			for (int r = 0; r < 4; ++r)
				result.m[c][r] = m.m[0][r] * n.m[c][0] + m.m[1][r] * n.m[c][1] + m.m[2][r] * n.m[c][2] + m.m[3][r] * n.m[c][3];

		return result;
	}

	Vector3 operator*(const Matrix4x4& m, const Vector3& v)
	{
		Vector3 result;

		result.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0];
		result.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1];
		result.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2];

		return result;
	}

	Matrix4x4 operator/(const Matrix4x4& m, double s)
	{
		Matrix4x4 result;

		for (int c = 0; c < 4; ++c)
			for (int r = 0; r < 4; ++r)
				result.m[c][r] = m.m[c][r] / s;

		return result;
	}

	Matrix4x4 operator-(const Matrix4x4& m)
	{
		Matrix4x4 result;

		for (int c = 0; c < 4; ++c)
			for (int r = 0; r < 4; ++r)
				result.m[c][r] = -m.m[c][r];

		return result;
	}

	bool operator==(const Matrix4x4& m, const Matrix4x4& n)
	{
		for (int c = 0; c < 4; ++c)
			for (int r = 0; r < 4; ++r)
				if (!MathUtils::almostSame(m.m[c][r], n.m[c][r]))
					return false;

		return true;
	}

	bool operator!=(const Matrix4x4& m, const Matrix4x4& n)
	{
		return !(m == n);
	}
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& n)
{
	*this = *this + n;

	return *this;
}

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& n)
{
	*this = *this - n;

	return *this;
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& n)
{
	*this = *this * n;

	return *this;
}

Matrix4x4& Matrix4x4::operator*=(double s)
{
	*this = *this * s;

	return *this;
}

Matrix4x4& Matrix4x4::operator/=(double s)
{
	*this = *this / s;

	return *this;
}

Matrix4x4::operator double*()
{
	return &m[0][0];
}

Matrix4x4::operator const double*() const
{
	return &m[0][0];
}

void Matrix4x4::transpose()
{
	*this = transposed();
}

Matrix4x4 Matrix4x4::transposed() const
{
	Matrix4x4 r;

	r.m[0][0] = m[0][0]; r.m[1][0] = m[0][1]; r.m[2][0] = m[0][2]; r.m[3][0] = m[0][3];
	r.m[0][1] = m[1][0]; r.m[1][1] = m[1][1]; r.m[2][1] = m[1][2]; r.m[3][1] = m[1][3];
	r.m[0][2] = m[2][0]; r.m[1][2] = m[2][1]; r.m[2][2] = m[2][2]; r.m[3][2] = m[2][3];
	r.m[0][3] = m[3][0]; r.m[1][3] = m[3][1]; r.m[2][3] = m[3][2]; r.m[3][3] = m[3][3];

	return r;
}

void Matrix4x4::invert()
{
	*this = inverted();
}

Matrix4x4 Matrix4x4::inverted() const
{
	double n[16], inv[16], out[16];

	std::memcpy(n, m, sizeof(double) * 16);

	inv[0] = n[5] * n[10] * n[15] -
		n[5] * n[11] * n[14] -
		n[9] * n[6] * n[15] +
		n[9] * n[7] * n[14] +
		n[13] * n[6] * n[11] -
		n[13] * n[7] * n[10];

	inv[4] = -n[4] * n[10] * n[15] +
		n[4] * n[11] * n[14] +
		n[8] * n[6] * n[15] -
		n[8] * n[7] * n[14] -
		n[12] * n[6] * n[11] +
		n[12] * n[7] * n[10];

	inv[8] = n[4] * n[9] * n[15] -
		n[4] * n[11] * n[13] -
		n[8] * n[5] * n[15] +
		n[8] * n[7] * n[13] +
		n[12] * n[5] * n[11] -
		n[12] * n[7] * n[9];

	inv[12] = -n[4] * n[9] * n[14] +
		n[4] * n[10] * n[13] +
		n[8] * n[5] * n[14] -
		n[8] * n[6] * n[13] -
		n[12] * n[5] * n[10] +
		n[12] * n[6] * n[9];

	inv[1] = -n[1] * n[10] * n[15] +
		n[1] * n[11] * n[14] +
		n[9] * n[2] * n[15] -
		n[9] * n[3] * n[14] -
		n[13] * n[2] * n[11] +
		n[13] * n[3] * n[10];

	inv[5] = n[0] * n[10] * n[15] -
		n[0] * n[11] * n[14] -
		n[8] * n[2] * n[15] +
		n[8] * n[3] * n[14] +
		n[12] * n[2] * n[11] -
		n[12] * n[3] * n[10];

	inv[9] = -n[0] * n[9] * n[15] +
		n[0] * n[11] * n[13] +
		n[8] * n[1] * n[15] -
		n[8] * n[3] * n[13] -
		n[12] * n[1] * n[11] +
		n[12] * n[3] * n[9];

	inv[13] = n[0] * n[9] * n[14] -
		n[0] * n[10] * n[13] -
		n[8] * n[1] * n[14] +
		n[8] * n[2] * n[13] +
		n[12] * n[1] * n[10] -
		n[12] * n[2] * n[9];

	inv[2] = n[1] * n[6] * n[15] -
		n[1] * n[7] * n[14] -
		n[5] * n[2] * n[15] +
		n[5] * n[3] * n[14] +
		n[13] * n[2] * n[7] -
		n[13] * n[3] * n[6];

	inv[6] = -n[0] * n[6] * n[15] +
		n[0] * n[7] * n[14] +
		n[4] * n[2] * n[15] -
		n[4] * n[3] * n[14] -
		n[12] * n[2] * n[7] +
		n[12] * n[3] * n[6];

	inv[10] = n[0] * n[5] * n[15] -
		n[0] * n[7] * n[13] -
		n[4] * n[1] * n[15] +
		n[4] * n[3] * n[13] +
		n[12] * n[1] * n[7] -
		n[12] * n[3] * n[5];

	inv[14] = -n[0] * n[5] * n[14] +
		n[0] * n[6] * n[13] +
		n[4] * n[1] * n[14] -
		n[4] * n[2] * n[13] -
		n[12] * n[1] * n[6] +
		n[12] * n[2] * n[5];

	inv[3] = -n[1] * n[6] * n[11] +
		n[1] * n[7] * n[10] +
		n[5] * n[2] * n[11] -
		n[5] * n[3] * n[10] -
		n[9] * n[2] * n[7] +
		n[9] * n[3] * n[6];

	inv[7] = n[0] * n[6] * n[11] -
		n[0] * n[7] * n[10] -
		n[4] * n[2] * n[11] +
		n[4] * n[3] * n[10] +
		n[8] * n[2] * n[7] -
		n[8] * n[3] * n[6];

	inv[11] = -n[0] * n[5] * n[11] +
		n[0] * n[7] * n[9] +
		n[4] * n[1] * n[11] -
		n[4] * n[3] * n[9] -
		n[8] * n[1] * n[7] +
		n[8] * n[3] * n[5];

	inv[15] = n[0] * n[5] * n[10] -
		n[0] * n[6] * n[9] -
		n[4] * n[1] * n[10] +
		n[4] * n[2] * n[9] +
		n[8] * n[1] * n[6] -
		n[8] * n[2] * n[5];

	Matrix4x4 result;

	double det = n[0] * inv[0] + n[1] * inv[4] + n[2] * inv[8] + n[3] * inv[12];

	if (det == 0)
		return result;

	det = 1.0 / det;

	for (int i = 0; i < 16; i++)
		out[i] = inv[i] * det;

	std::memcpy(result.m, out, sizeof(double) * 16);

	return result;
}

Vector3 Matrix4x4::transformPosition(const Vector3& v) const
{
	return (*this * v);
}

Vector3 Matrix4x4::transformDirection(const Vector3& v) const
{
	Vector3 result;

	result.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z;
	result.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z;
	result.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z;

	return result;
}

Matrix4x4 Matrix4x4::scale(const Vector3& s)
{
	return scale(s.x, s.y, s.z);
}

Matrix4x4 Matrix4x4::scale(double sx, double sy, double sz)
{
	Matrix4x4 result = IDENTITY;

	result.m[0][0] = sx;
	result.m[1][1] = sy;
	result.m[2][2] = sz;

	return result;
}

Matrix4x4 Matrix4x4::translate(const Vector3& t)
{
	return translate(t.x, t.y, t.z);
}

Matrix4x4 Matrix4x4::translate(double tx, double ty, double tz)
{
	Matrix4x4 result = IDENTITY;

	result.m[3][0] = tx;
	result.m[3][1] = ty;
	result.m[3][2] = tz;

	return result;
}

Matrix4x4 Matrix4x4::rotateXYZ(const EulerAngle& e)
{
	return rotateXYZ(e.pitch, e.yaw, e.roll);
}

Matrix4x4 Matrix4x4::rotateZYX(const EulerAngle& e)
{
	return rotateZYX(e.pitch, e.yaw, e.roll);
}

Matrix4x4 Matrix4x4::rotateXYZ(double pitch, double yaw, double roll)
{
	return rotateX(pitch) * rotateY(yaw) * rotateZ(roll);
}

Matrix4x4 Matrix4x4::rotateZYX(double pitch, double yaw, double roll)
{
	return rotateZ(roll) * rotateY(yaw) * rotateX(pitch);
}

Matrix4x4 Matrix4x4::rotateX(double degrees)
{
	Matrix4x4 result = IDENTITY;

	double sine = sin(MathUtils::degToRad(degrees));
	double cosine = cos(MathUtils::degToRad(degrees));

	result.m[1][1] = cosine;
	result.m[2][1] = -sine;
	result.m[1][2] = sine;
	result.m[2][2] = cosine;

	return result;
}

Matrix4x4 Matrix4x4::rotateY(double degrees)
{
	Matrix4x4 result = IDENTITY;

	double sine = sin(MathUtils::degToRad(degrees));
	double cosine = cos(MathUtils::degToRad(degrees));

	result.m[0][0] = cosine;
	result.m[2][0] = sine;
	result.m[0][2] = -sine;
	result.m[2][2] = cosine;

	return result;
}

Matrix4x4 Matrix4x4::rotateZ(double degrees)
{
	Matrix4x4 result = IDENTITY;

	double sine = sin(MathUtils::degToRad(degrees));
	double cosine = cos(MathUtils::degToRad(degrees));

	result.m[0][0] = cosine;
	result.m[1][0] = -sine;
	result.m[0][1] = sine;
	result.m[1][1] = cosine;

	return result;
}

// http://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
Matrix4x4 Matrix4x4::rotate(const Vector3& from, const Vector3& to)
{
	Vector3 v = from.cross(to);
	double s = v.length();
	double c = from.dot(to);

	Matrix4x4 vx = Matrix4x4(
		0.0, -v.z, v.y, 0.0,
		v.z, 0.0, -v.x, 0.0,
		-v.y, v.x, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0);

	if (s != 0.0)
		return IDENTITY + vx + (vx * vx) * ((1.0 - c) / (s * s));
	else
		return IDENTITY;
}
