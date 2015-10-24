// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

/*

https://en.wikipedia.org/wiki/Row-major_order

row major

array indices:
m[row][column]
[00, 01, 02, 03]
[10, 11, 12, 13]
[20, 21, 22, 23]
[30, 31, 32, 33]
 R   U   F   T

R = [1 0 0] (x, right)
U = [0 1 0] (y, up)
F = [0 0 1] (z, forward, out of monitor, right-handed coordinate system)
T = translation

*/

namespace Raycer
{
	class Vector3;
	class Vector4;
	class EulerAngle;

	class Matrix4x4
	{
	public:

		Matrix4x4();
		Matrix4x4(double m00, double m01, double m02, double m03, double m10, double m11, double m12, double m13, double m20, double m21, double m22, double m23, double m30, double m31, double m32, double m33);
		Matrix4x4(const Vector4& r, const Vector4& u, const Vector4& f, const Vector4& t);

		friend Matrix4x4 operator+(const Matrix4x4& m, const Matrix4x4& n);
		friend Matrix4x4 operator-(const Matrix4x4& m, const Matrix4x4& n);
		friend Matrix4x4 operator*(const Matrix4x4& m, double s);
		friend Matrix4x4 operator*(double s, const Matrix4x4& m);
		friend Matrix4x4 operator*(const Matrix4x4& m, const Matrix4x4& n);
		friend Vector4 operator*(const Matrix4x4& m, const Vector4& v);
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

		double get(uint64_t row, uint64_t column) const;
		void set(uint64_t row, uint64_t column, double value);
		Vector4 getRow(uint64_t index) const;
		void setRow(uint64_t index, const Vector4& v);
		Vector4 getColumn(uint64_t index) const;
		void setColumn(uint64_t index, const Vector4& v);

		void transpose();
		Matrix4x4 transposed() const;
		void invert();
		Matrix4x4 inverted() const;
		bool isZero() const;
		bool isNan() const;

		Vector3 transformPosition(const Vector3& v) const;
		Vector3 transformDirection(const Vector3& v) const;

		static const Matrix4x4 IDENTITY;
		static const Matrix4x4 ZERO;

		static Matrix4x4 scale(const Vector3& s);
		static Matrix4x4 scale(double sx, double sy, double sz);
		static Matrix4x4 translate(const Vector3& t);
		static Matrix4x4 translate(double tx, double ty, double tz);
		static Matrix4x4 rotateXYZ(const EulerAngle& e);
		static Matrix4x4 rotateZYX(const EulerAngle& e);
		static Matrix4x4 rotateXYZ(double pitch, double yaw, double roll);
		static Matrix4x4 rotateZYX(double pitch, double yaw, double roll);
		static Matrix4x4 rotateX(double angle);
		static Matrix4x4 rotateY(double angle);
		static Matrix4x4 rotateZ(double angle);
		static Matrix4x4 rotate(const Vector3& from, const Vector3& to);

		double m[4][4];

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(m));
		}
	};
}
