// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Vector2
	{
	public:

		Vector2(float x = 0.0f, float y = 0.0f);
		Vector2(const Vector2& v);
		Vector2& operator=(const Vector2& v);

		friend Vector2 operator+(const Vector2& v, const Vector2& w);
		friend Vector2 operator-(const Vector2& v, const Vector2& w);
		friend Vector2 operator*(const Vector2& v, float s);
		friend Vector2 operator*(float s, const Vector2& v);
		friend Vector2 operator/(const Vector2& v, float s);
		friend Vector2 operator-(const Vector2& v);

		friend bool operator==(const Vector2& v, const Vector2& w);
		friend bool operator!=(const Vector2& v, const Vector2& w);

		Vector2& operator+=(const Vector2& v);
		Vector2& operator-=(const Vector2& v);
		Vector2& operator*=(float s);
		Vector2& operator/=(float s);

		float length() const;
		float lengthSquared() const;
		void normalize();
		Vector2 normalized() const;
		bool isZero() const;
		bool isNormal() const;
		float dot(const Vector2& v) const;
		Vector2 reflect(const Vector2& normal) const;
		static Vector2 lerp(const Vector2& v1, const Vector2& v2, float t);

		float x;
		float y;
	};
}
