// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

namespace Raycer
{
	class Vector3
	{
	public:

		Vector3(double x = 0.0, double y = 0.0, double z = 0.0);
		Vector3(const Vector3& v);
		Vector3& operator=(const Vector3& v);

		friend Vector3 operator+(const Vector3& v, const Vector3& w);
		friend Vector3 operator-(const Vector3& v, const Vector3& w);
		friend Vector3 operator*(const Vector3& v, const Vector3& w);
		friend Vector3 operator*(const Vector3& v, double s);
		friend Vector3 operator*(double s, const Vector3& v);
		friend Vector3 operator/(const Vector3& v, const Vector3& w);
		friend Vector3 operator/(const Vector3& v, double s);
		friend Vector3 operator-(const Vector3& v);

		friend bool operator==(const Vector3& v, const Vector3& w);
		friend bool operator!=(const Vector3& v, const Vector3& w);
		friend bool operator>(const Vector3& v, const Vector3& w);
		friend bool operator<(const Vector3& v, const Vector3& w);

		Vector3& operator+=(const Vector3& v);
		Vector3& operator-=(const Vector3& v);
		Vector3& operator*=(const Vector3& v);
		Vector3& operator*=(double s);
		Vector3& operator/=(const Vector3& v);
		Vector3& operator/=(double s);

		double length() const;
		double lengthSquared() const;
		void normalize();
		Vector3 normalized() const;
		void inverse();
		Vector3 inversed() const;
		bool isZero() const;
		bool isNormal() const;
		double dot(const Vector3& v) const;
		Vector3 cross(const Vector3& v) const;
		Vector3 reflect(const Vector3& normal) const;
		std::string toString() const;
		double element(int number) const;

		static Vector3 lerp(const Vector3& v1, const Vector3& v2, double t);
		static Vector3 abs(const Vector3& v);

		static const Vector3 RIGHT;   // [1 0 0]
		static const Vector3 UP;      // [0 1 0]
		static const Vector3 FORWARD; // [0 0 1]
		static const Vector3 ALMOST_UP;

		double x;
		double y;
		double z;
	};
}
