// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

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
	class Vector3;
	class EulerAngle;

	class Matrix4x4
	{
	public:

		Matrix4x4();
		Matrix4x4(const Matrix4x4& m);
		Matrix4x4(double m00, double m10, double m20, double m30, double m01, double m11, double m21, double m31, double m02, double m12, double m22, double m32, double m03, double m13, double m23, double m33);
		Matrix4x4& operator=(const Matrix4x4& m);

		friend Matrix4x4 operator+(const Matrix4x4& m, const Matrix4x4& n);
		friend Matrix4x4 operator-(const Matrix4x4& m, const Matrix4x4& n);
		friend Matrix4x4 operator*(const Matrix4x4& m, double s);
		friend Matrix4x4 operator*(double s, const Matrix4x4& m);
		friend Matrix4x4 operator*(const Matrix4x4& m, const Matrix4x4& n);
		friend Vector3 operator*(const Matrix4x4& m, const Vector3& v);
		friend Matrix4x4 operator/(const Matrix4x4& m, double s);
		friend Matrix4x4 operator-(const Matrix4x4& m);

		friend bool operator==(const Matrix4x4& m, const Matrix4x4& n);
		friend bool operator!=(const Matrix4x4& m, const Matrix4x4& n);

		Matrix4x4& operator+=(const Matrix4x4& m);
		Matrix4x4& operator-=(const Matrix4x4& m);
		Matrix4x4& operator*=(const Matrix4x4& m);
		Matrix4x4& operator*=(double s);
		Matrix4x4& operator/=(double s);

		operator double*();
		operator const double*() const;

		void transpose();
		Matrix4x4 transposed() const;

		Vector3 transformPosition(const Vector3& v) const;
		Vector3 transformDirection(const Vector3& v) const;

		static const Matrix4x4 IDENTITY;
		static const Matrix4x4 ZERO;

		static Matrix4x4 scale(const Vector3& s);
		static Matrix4x4 scale(double sx, double sy, double sz);

		static Matrix4x4 translate(const Vector3& t);
		static Matrix4x4 translate(double tx, double ty, double tz);

		static Matrix4x4 rotateXYZ(double pitch, double yaw, double roll);
		static Matrix4x4 rotateX(double degrees);
		static Matrix4x4 rotateY(double degrees);
		static Matrix4x4 rotateZ(double degrees);

		double m[4][4];
	};
}
