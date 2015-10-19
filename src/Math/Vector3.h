// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

namespace Raycer
{
	class Vector4;

	class Vector3
	{
	public:

		explicit Vector3(double x = 0.0, double y = 0.0, double z = 0.0);
		explicit Vector3(const Vector4& v);

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

		double get(uint index) const;
		void set(uint index, double value);
		double length() const;
		double lengthSquared() const;
		void normalize();
		Vector3 normalized() const;
		void inverse();
		Vector3 inversed() const;
		bool isZero() const;
		bool isNan() const;
		bool isNormal() const;
		double dot(const Vector3& v) const;
		Vector3 cross(const Vector3& v) const;
		Vector3 reflect(const Vector3& normal) const;
		std::string toString() const;
		Vector4 toVector4(double w = 0.0) const;

		static Vector3 lerp(const Vector3& v1, const Vector3& v2, double t);
		static Vector3 abs(const Vector3& v);

		static const Vector3 RIGHT;
		static const Vector3 UP;
		static const Vector3 FORWARD;
		static const Vector3 ALMOST_UP;

		double x;
		double y;
		double z;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(x),
				CEREAL_NVP(y),
				CEREAL_NVP(z));
		}
	};
}
