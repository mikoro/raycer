// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

namespace Raycer
{
	class Vector2
	{
	public:

		explicit Vector2(double x = 0.0, double y = 0.0);

		friend Vector2 operator+(const Vector2& v, const Vector2& w);
		friend Vector2 operator-(const Vector2& v, const Vector2& w);
		friend Vector2 operator*(const Vector2& v, const Vector2& w);
		friend Vector2 operator*(const Vector2& v, double s);
		friend Vector2 operator*(double s, const Vector2& v);
		friend Vector2 operator/(const Vector2& v, const Vector2& w);
		friend Vector2 operator/(const Vector2& v, double s);
		friend Vector2 operator-(const Vector2& v);

		friend bool operator==(const Vector2& v, const Vector2& w);
		friend bool operator!=(const Vector2& v, const Vector2& w);
		friend bool operator>(const Vector2& v, const Vector2& w);
		friend bool operator<(const Vector2& v, const Vector2& w);

		Vector2& operator+=(const Vector2& v);
		Vector2& operator-=(const Vector2& v);
		Vector2& operator*=(const Vector2& v);
		Vector2& operator*=(double s);
		Vector2& operator/=(const Vector2& v);
		Vector2& operator/=(double s);

		double get(uint64_t index) const;
		void set(uint64_t index, double value);
		double length() const;
		double lengthSquared() const;
		void normalize();
		Vector2 normalized() const;
		void inverse();
		Vector2 inversed() const;
		bool isZero() const;
		bool isNan() const;
		bool isNormal() const;
		double dot(const Vector2& v) const;
		Vector2 reflect(const Vector2& normal) const;
		std::string toString() const;

		static Vector2 lerp(const Vector2& v1, const Vector2& v2, double t);
		static Vector2 abs(const Vector2& v);

		double x;
		double y;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(x),
				CEREAL_NVP(y));
		}
	};
}
