// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/EulerAngle.h"
#include "Math/MathUtils.h"

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

Matrix4x4::Matrix4x4(double m00, double m01, double m02, double m03, double m10, double m11, double m12, double m13, double m20, double m21, double m22, double m23, double m30, double m31, double m32, double m33)
{
	m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
	m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
	m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
	m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
}

Matrix4x4::Matrix4x4(const Vector4& r, const Vector4& u, const Vector4& f, const Vector4& t)
{
	m[0][0] = r.x; m[0][1] = u.x; m[0][2] = f.x; m[0][3] = t.x;
	m[1][0] = r.y; m[1][1] = u.y; m[1][2] = f.y; m[1][3] = t.y;
	m[2][0] = r.z; m[2][1] = u.z; m[2][2] = f.z; m[2][3] = t.z;
	m[3][0] = r.w; m[3][1] = u.w; m[3][2] = f.w; m[3][3] = t.w;
}

namespace Raycer
{
	Matrix4x4 operator+(const Matrix4x4& m, const Matrix4x4& n)
	{
		Matrix4x4 result;

		for (uint r = 0; r <= 3; ++r)
			for (uint c = 0; c <= 3; ++c)
				result.m[r][c] = m.m[r][c] + n.m[r][c];

		return result;
	}

	Matrix4x4 operator-(const Matrix4x4& m, const Matrix4x4& n)
	{
		Matrix4x4 result;

		for (uint r = 0; r <= 3; ++r)
			for (uint c = 0; c <= 3; ++c)
				result.m[r][c] = m.m[r][c] - n.m[r][c];

		return result;
	}

	Matrix4x4 operator*(const Matrix4x4& m, double s)
	{
		Matrix4x4 result;

		for (uint r = 0; r <= 3; ++r)
			for (uint c = 0; c <= 3; ++c)
				result.m[r][c] = m.m[r][c] * s;

		return result;
	}

	Matrix4x4 operator*(double s, const Matrix4x4& m)
	{
		Matrix4x4 result;

		for (uint r = 0; r <= 3; ++r)
			for (uint c = 0; c <= 3; ++c)
				result.m[r][c] = m.m[r][c] * s;

		return result;
	}

	Matrix4x4 operator*(const Matrix4x4& m, const Matrix4x4& n)
	{
		Matrix4x4 result;

		for (uint r = 0; r <= 3; ++r)
			for (uint c = 0; c <= 3; ++c)
				result.m[r][c] = m.m[r][0] * n.m[0][c] + m.m[r][1] * n.m[1][c] + m.m[r][2] * n.m[2][c] + m.m[r][3] * n.m[3][c];

		return result;
	}

	Vector4 operator*(const Matrix4x4& m, const Vector4& v)
	{
		Vector4 result;

		result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
		result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
		result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
		result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

		return result;
	}

	Matrix4x4 operator/(const Matrix4x4& m, double s)
	{
		Matrix4x4 result;

		for (uint r = 0; r <= 3; ++r)
			for (uint c = 0; c <= 3; ++c)
				result.m[r][c] = m.m[r][c] / s;

		return result;
	}

	Matrix4x4 operator-(const Matrix4x4& m)
	{
		Matrix4x4 result;

		for (uint r = 0; r <= 3; ++r)
			for (uint c = 0; c <= 3; ++c)
				result.m[r][c] = -m.m[r][c];

		return result;
	}

	bool operator==(const Matrix4x4& m, const Matrix4x4& n)
	{
		for (uint r = 0; r <= 3; ++r)
			for (uint c = 0; c <= 3; ++c)
				if (!MathUtils::almostSame(m.m[r][c], n.m[r][c]))
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

double Matrix4x4::get(uint row, uint column) const
{
	assert(row <= 3 && column <= 3);
	return m[row][column];
}

void Matrix4x4::set(uint row, uint column, double value)
{
	assert(row <= 3 && column <= 3);
	m[row][column] = value;
}

Vector4 Matrix4x4::getRow(uint index) const
{
	assert(index <= 3);
	return Vector4(m[index][0], m[index][1], m[index][2], m[index][3]);
}

void Matrix4x4::setRow(uint index, const Vector4& v)
{
	assert(index <= 3);

	m[index][0] = v.x;
	m[index][1] = v.y;
	m[index][2] = v.z;
	m[index][3] = v.w;
}

Vector4 Matrix4x4::getColumn(uint index) const
{
	assert(index <= 3);
	return Vector4(m[0][index], m[1][index], m[2][index], m[3][index]);
}

void Matrix4x4::setColumn(uint index, const Vector4& v)
{
	assert(index <= 3);

	m[0][index] = v.x;
	m[1][index] = v.y;
	m[2][index] = v.z;
	m[3][index] = v.w;
}

void Matrix4x4::transpose()
{
	*this = transposed();
}

Matrix4x4 Matrix4x4::transposed() const
{
	Matrix4x4 result;

	for (uint r = 0; r <= 3; ++r)
		for (uint c = 0; c <= 3; ++c)
			result.m[r][c] = m[c][r];

	return result;
}

void Matrix4x4::invert()
{
	*this = inverted();
}

Matrix4x4 Matrix4x4::inverted() const
{
	double n[16], inv[16], out[16];
	std::memcpy(n, m, sizeof(double) * 16);

	inv[0] = n[5] * n[10] * n[15] - n[5] * n[11] * n[14] - n[9] * n[6] * n[15] + n[9] * n[7] * n[14] + n[13] * n[6] * n[11] - n[13] * n[7] * n[10];
	inv[4] = -n[4] * n[10] * n[15] + n[4] * n[11] * n[14] + n[8] * n[6] * n[15] - n[8] * n[7] * n[14] - n[12] * n[6] * n[11] + n[12] * n[7] * n[10];
	inv[8] = n[4] * n[9] * n[15] - n[4] * n[11] * n[13] - n[8] * n[5] * n[15] + n[8] * n[7] * n[13] + n[12] * n[5] * n[11] - n[12] * n[7] * n[9];
	inv[12] = -n[4] * n[9] * n[14] + n[4] * n[10] * n[13] + n[8] * n[5] * n[14] - n[8] * n[6] * n[13] - n[12] * n[5] * n[10] + n[12] * n[6] * n[9];
	inv[1] = -n[1] * n[10] * n[15] + n[1] * n[11] * n[14] + n[9] * n[2] * n[15] - n[9] * n[3] * n[14] - n[13] * n[2] * n[11] + n[13] * n[3] * n[10];
	inv[5] = n[0] * n[10] * n[15] - n[0] * n[11] * n[14] - n[8] * n[2] * n[15] + n[8] * n[3] * n[14] + n[12] * n[2] * n[11] - n[12] * n[3] * n[10];
	inv[9] = -n[0] * n[9] * n[15] + n[0] * n[11] * n[13] + n[8] * n[1] * n[15] - n[8] * n[3] * n[13] - n[12] * n[1] * n[11] + n[12] * n[3] * n[9];
	inv[13] = n[0] * n[9] * n[14] - n[0] * n[10] * n[13] - n[8] * n[1] * n[14] + n[8] * n[2] * n[13] + n[12] * n[1] * n[10] - n[12] * n[2] * n[9];
	inv[2] = n[1] * n[6] * n[15] - n[1] * n[7] * n[14] - n[5] * n[2] * n[15] + n[5] * n[3] * n[14] + n[13] * n[2] * n[7] - n[13] * n[3] * n[6];
	inv[6] = -n[0] * n[6] * n[15] + n[0] * n[7] * n[14] + n[4] * n[2] * n[15] - n[4] * n[3] * n[14] - n[12] * n[2] * n[7] + n[12] * n[3] * n[6];
	inv[10] = n[0] * n[5] * n[15] - n[0] * n[7] * n[13] - n[4] * n[1] * n[15] + n[4] * n[3] * n[13] + n[12] * n[1] * n[7] - n[12] * n[3] * n[5];
	inv[14] = -n[0] * n[5] * n[14] + n[0] * n[6] * n[13] + n[4] * n[1] * n[14] - n[4] * n[2] * n[13] - n[12] * n[1] * n[6] + n[12] * n[2] * n[5];
	inv[3] = -n[1] * n[6] * n[11] + n[1] * n[7] * n[10] + n[5] * n[2] * n[11] - n[5] * n[3] * n[10] - n[9] * n[2] * n[7] + n[9] * n[3] * n[6];
	inv[7] = n[0] * n[6] * n[11] - n[0] * n[7] * n[10] - n[4] * n[2] * n[11] + n[4] * n[3] * n[10] + n[8] * n[2] * n[7] - n[8] * n[3] * n[6];
	inv[11] = -n[0] * n[5] * n[11] + n[0] * n[7] * n[9] + n[4] * n[1] * n[11] - n[4] * n[3] * n[9] - n[8] * n[1] * n[7] + n[8] * n[3] * n[5];
	inv[15] = n[0] * n[5] * n[10] - n[0] * n[6] * n[9] - n[4] * n[1] * n[10] + n[4] * n[2] * n[9] + n[8] * n[1] * n[6] - n[8] * n[2] * n[5];

	Matrix4x4 result;

	double det = n[0] * inv[0] + n[1] * inv[4] + n[2] * inv[8] + n[3] * inv[12];

	if (det == 0.0)
		return result;

	det = 1.0 / det;

	for (uint i = 0; i < 16; i++)
		out[i] = inv[i] * det;

	std::memcpy(result.m, out, sizeof(double) * 16);
	return result;
}

bool Matrix4x4::isZero() const
{
	for (uint r = 0; r <= 3; ++r)
	{
		for (uint c = 0; c <= 3; ++c)
		{
			if (m[r][c] != 0.0)
				return false;
		}
	}

	return true;
}

bool Matrix4x4::isNan() const
{
	for (uint r = 0; r <= 3; ++r)
	{
		for (uint c = 0; c <= 3; ++c)
		{
			if (std::isnan(m[r][c]))
				return true;
		}
	}

	return false;
}

Vector3 Matrix4x4::transformPosition(const Vector3& v) const
{
	Vector3 result;

	result.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3];
	result.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3];
	result.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3];

	return result;
}

Vector3 Matrix4x4::transformDirection(const Vector3& v) const
{
	Vector3 result;

	result.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
	result.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
	result.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;

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

	result.m[0][3] = tx;
	result.m[1][3] = ty;
	result.m[2][3] = tz;

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
	result.m[1][2] = -sine;
	result.m[2][1] = sine;
	result.m[2][2] = cosine;

	return result;
}

Matrix4x4 Matrix4x4::rotateY(double degrees)
{
	Matrix4x4 result = IDENTITY;

	double sine = sin(MathUtils::degToRad(degrees));
	double cosine = cos(MathUtils::degToRad(degrees));

	result.m[0][0] = cosine;
	result.m[0][2] = sine;
	result.m[2][0] = -sine;
	result.m[2][2] = cosine;

	return result;
}

Matrix4x4 Matrix4x4::rotateZ(double degrees)
{
	Matrix4x4 result = IDENTITY;

	double sine = sin(MathUtils::degToRad(degrees));
	double cosine = cos(MathUtils::degToRad(degrees));

	result.m[0][0] = cosine;
	result.m[0][1] = -sine;
	result.m[1][0] = sine;
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
