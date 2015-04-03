// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>
#include <cstring>

#include "Math/Matrix4x4.h"
#include "Math/MathUtils.h"

using namespace Raycer;

const Matrix4x4 Matrix4x4::IDENTITY = Matrix4x4(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

const Matrix4x4 Matrix4x4::ZERO = Matrix4x4();

Matrix4x4::Matrix4x4()
{
	std::memset(m, 0, sizeof(float) * 16);
}

Matrix4x4::Matrix4x4(const Matrix4x4& n)
{
	std::memcpy(m, n.m, sizeof(float) * 16);
}

Matrix4x4::Matrix4x4(float m00, float m10, float m20, float m30, float m01, float m11, float m21, float m31, float m02, float m12, float m22, float m32, float m03, float m13, float m23, float m33)
{
	m[0][0] = m00; m[1][0] = m10; m[2][0] = m20; m[3][0] = m30;
	m[0][1] = m01; m[1][1] = m11; m[2][1] = m21; m[3][1] = m31;
	m[0][2] = m02; m[1][2] = m12; m[2][2] = m22; m[3][2] = m32;
	m[0][3] = m03; m[1][3] = m13; m[2][3] = m23; m[3][3] = m33;
}

Matrix4x4& Matrix4x4::operator=(const Matrix4x4& n)
{
	std::memcpy(m, n.m, sizeof(float) * 16);

	return *this;
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& n) const
{
	Matrix4x4 result;

	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			result.m[c][r] = m[c][r] + n.m[c][r];

	return result;
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& n) const
{
	Matrix4x4 result;

	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			result.m[c][r] = m[c][r] - n.m[c][r];

	return result;
}

Matrix4x4 Matrix4x4::operator*(float s) const
{
	Matrix4x4 result;

	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			result.m[c][r] = m[c][r] * s;

	return result;
}

Matrix4x4 Matrix4x4::operator/(float s) const
{
	Matrix4x4 result;

	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			result.m[c][r] = m[c][r] / s;

	return result;
}

Matrix4x4 Matrix4x4::operator-() const
{
	Matrix4x4 result;

	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			result.m[c][r] = -m[c][r];

	return result;
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& n)
{
	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			m[c][r] += n.m[c][r];

	return *this;
}

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& n)
{
	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			m[c][r] -= n.m[c][r];

	return *this;
}

Matrix4x4& Matrix4x4::operator*=(float s)
{
	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			m[c][r] *= s;

	return *this;
}

Matrix4x4& Matrix4x4::operator/=(float s)
{
	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			m[c][r] /= s;

	return *this;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& n) const
{
	Matrix4x4 result;

	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			result.m[c][r] = m[0][r] * n.m[c][0] + m[1][r] * n.m[c][1] + m[2][r] * n.m[c][2] + m[3][r] * n.m[c][3];

	return result;
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& n)
{
	*this = *this * n;

	return *this;
}

Vector3 Matrix4x4::operator*(const Vector3& v) const
{
	Vector3 r;

	r.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0];
	r.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1];
	r.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2];

	return r;
}

bool Matrix4x4::operator==(const Matrix4x4& n) const
{
	for (int c = 0; c < 4; ++c)
		for (int r = 0; r < 4; ++r)
			if (!MathUtils::almostSame(m[c][r], n.m[c][r]))
				return false;

	return true;
}

bool Matrix4x4::operator!=(const Matrix4x4& n) const
{
	return !(*this == n);
}

Matrix4x4::operator float*()
{
	return &m[0][0];
}

Matrix4x4::operator const float*() const
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
