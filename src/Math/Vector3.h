// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Vector3
	{
	public:

		Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
		Vector3(const Vector3& v);
		Vector3& operator=(const Vector3& v);

		friend Vector3 operator+(const Vector3& v, const Vector3& w);
		friend Vector3 operator-(const Vector3& v, const Vector3& w);
		friend Vector3 operator*(const Vector3& v, float s);
		friend Vector3 operator*(float s, const Vector3& v);
		friend Vector3 operator/(const Vector3& v, float s);
		friend Vector3 operator-(const Vector3& v);

		friend bool operator==(const Vector3& v, const Vector3& w);
		friend bool operator!=(const Vector3& v, const Vector3& w);

		Vector3& operator+=(const Vector3& v);
		Vector3& operator-=(const Vector3& v);
		Vector3& operator*=(float s);
		Vector3& operator/=(float s);

		float length() const;
		float lengthSquared() const;
		void normalize();
		Vector3 normalized() const;
		bool isZero() const;
		bool isNormal() const;
		float dot(const Vector3& v) const;
		Vector3 cross(const Vector3& v) const;
		Vector3 reflect(const Vector3& normal) const;
		static Vector3 lerp(const Vector3& v1, const Vector3& v2, float t);

		static const Vector3 RIGHT;   // [1 0 0]
		static const Vector3 UP;      // [0 1 0]
		static const Vector3 FORWARD; // [0 0 1]

		float x;
		float y;
		float z;
	};
}
