// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>
#include <cstring>

#include "Math/Matrix4x4.h"
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

Matrix4x4& Matrix4x4::operator=(const Matrix4x4& n)
{
	std::memcpy(m, n.m, sizeof(double) * 16);

	return *this;
}

Matrix4x4 Raycer::operator+(const Matrix4x4& m, const Matrix4x4& n)
{
	Matrix4x4 result;

	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			result.m[c][r] = m.m[c][r] + n.m[c][r];

	return result;
}

Matrix4x4 Raycer::operator-(const Matrix4x4& m, const Matrix4x4& n)
{
	Matrix4x4 result;

	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			result.m[c][r] = m.m[c][r] - n.m[c][r];

	return result;
}

Matrix4x4 Raycer::operator*(const Matrix4x4& m, double s)
{
	Matrix4x4 result;

	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			result.m[c][r] = m.m[c][r] * s;

	return result;
}

Matrix4x4 Raycer::operator*(double s, const Matrix4x4& m)
{
	Matrix4x4 result;

	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			result.m[c][r] = m.m[c][r] * s;

	return result;
}

Matrix4x4 Raycer::operator*(const Matrix4x4& m, const Matrix4x4& n)
{
	Matrix4x4 result;

	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			result.m[c][r] = m.m[0][r] * n.m[c][0] + m.m[1][r] * n.m[c][1] + m.m[2][r] * n.m[c][2] + m.m[3][r] * n.m[c][3];

	return result;
}

Vector3 Raycer::operator*(const Matrix4x4& m, const Vector3& v)
{
	Vector3 result;

	result.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0];
	result.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1];
	result.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2];

	return result;
}

Matrix4x4 Raycer::operator/(const Matrix4x4& m, double s)
{
	Matrix4x4 result;

	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			result.m[c][r] = m.m[c][r] / s;

	return result;
}

Matrix4x4 Raycer::operator-(const Matrix4x4& m)
{
	Matrix4x4 result;

	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			result.m[c][r] = -m.m[c][r];

	return result;
}

bool Raycer::operator==(const Matrix4x4& m, const Matrix4x4& n)
{
	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			if (!MathUtils::almostSame(m.m[c][r], n.m[c][r]))
				return false;

	return true;
}

bool Raycer::operator!=(const Matrix4x4& m, const Matrix4x4& n)
{
	return !(m == n);
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

Matrix4x4 Matrix4x4::scale(double sx, double sy, double sz)
{
	Matrix4x4 result = IDENTITY;

	result.m[0][0] = sx;
	result.m[1][1] = sy;
	result.m[2][2] = sz;

	return result;
}

Matrix4x4 Matrix4x4::translate(double tx, double ty, double tz)
{
	Matrix4x4 result = IDENTITY;

	result.m[3][0] = tx;
	result.m[3][1] = ty;
	result.m[3][2] = tz;

	return result;
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
