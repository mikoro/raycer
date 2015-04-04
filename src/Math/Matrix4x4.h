// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"

/*

column major

m[c][r]

 R   U   F   T
[00, 10, 20, 30]
[01, 11, 21, 31]
[02, 12, 22, 32]
[03, 13, 23, 33]

R = [1 0 0] (x, right)
U = [0 1 0] (y, up)
F = [0 0 1] (z, forward, out of monitor, right-handed coordinate system)
T = translation

*/

namespace Raycer
{
	class Matrix4x4
	{
	public:

		Matrix4x4();
		Matrix4x4(const Matrix4x4& m);
		Matrix4x4(float m00, float m10, float m20, float m30, float m01, float m11, float m21, float m31, float m02, float m12, float m22, float m32, float m03, float m13, float m23, float m33);
		Matrix4x4& operator=(const Matrix4x4& m);

		friend Matrix4x4 operator+(const Matrix4x4& m, const Matrix4x4& n);
		friend Matrix4x4 operator-(const Matrix4x4& m, const Matrix4x4& n);
		friend Matrix4x4 operator*(const Matrix4x4& m, float s);
		friend Matrix4x4 operator*(float s, const Matrix4x4& m);
		friend Matrix4x4 operator*(const Matrix4x4& m, const Matrix4x4& n);
		friend Vector3 operator*(const Matrix4x4& m, const Vector3& v);
		friend Matrix4x4 operator/(const Matrix4x4& m, float s);
		friend Matrix4x4 operator-(const Matrix4x4& m);

		friend bool operator==(const Matrix4x4& m, const Matrix4x4& n);
		friend bool operator!=(const Matrix4x4& m, const Matrix4x4& n);

		Matrix4x4& operator+=(const Matrix4x4& m);
		Matrix4x4& operator-=(const Matrix4x4& m);
		Matrix4x4& operator*=(const Matrix4x4& m);
		Matrix4x4& operator*=(float s);
		Matrix4x4& operator/=(float s);

		operator float*();
		operator const float*() const;

		void transpose();
		Matrix4x4 transposed() const;

		static const Matrix4x4 IDENTITY;
		static const Matrix4x4 ZERO;

		static Matrix4x4 scale(float sx, float sy, float sz);
		static Matrix4x4 translate(float tx, float ty, float tz);
		static Matrix4x4 rotateX(float degrees);
		static Matrix4x4 rotateY(float degrees);
		static Matrix4x4 rotateZ(float degrees);

		float m[4][4];
	};
}
