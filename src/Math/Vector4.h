// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

namespace Raycer
{
	class Vector3;

	class Vector4
	{
	public:

		explicit Vector4(double x = 0.0, double y = 0.0, double z = 0.0, double w = 0.0);
		explicit Vector4(const Vector3& v, double w = 0.0);

		friend Vector4 operator+(const Vector4& v, const Vector4& w);
		friend Vector4 operator-(const Vector4& v, const Vector4& w);
		friend Vector4 operator*(const Vector4& v, const Vector4& w);
		friend Vector4 operator*(const Vector4& v, double s);
		friend Vector4 operator*(double s, const Vector4& v);
		friend Vector4 operator/(const Vector4& v, const Vector4& w);
		friend Vector4 operator/(const Vector4& v, double s);
		friend Vector4 operator-(const Vector4& v);

		friend bool operator==(const Vector4& v, const Vector4& w);
		friend bool operator!=(const Vector4& v, const Vector4& w);
		friend bool operator>(const Vector4& v, const Vector4& w);
		friend bool operator<(const Vector4& v, const Vector4& w);

		Vector4& operator+=(const Vector4& v);
		Vector4& operator-=(const Vector4& v);
		Vector4& operator*=(const Vector4& v);
		Vector4& operator*=(double s);
		Vector4& operator/=(const Vector4& v);
		Vector4& operator/=(double s);

		double get(uint64_t index) const;
		void set(uint64_t index, double value);
		double length() const;
		double lengthSquared() const;
		void normalizeLength();
		Vector4 normalizedLength() const;
		void normalizeForm();
		Vector4 normalizedForm() const;
		void inverse();
		Vector4 inversed() const;
		bool isZero() const;
		bool isNan() const;
		bool isNormal() const;
		double dot(const Vector4& v) const;
		std::string toString() const;
		Vector3 toVector3() const;

		static Vector4 lerp(const Vector4& v1, const Vector4& v2, double t);
		static Vector4 abs(const Vector4& v);

		double x;
		double y;
		double z;
		double w;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(x),
				CEREAL_NVP(y),
				CEREAL_NVP(z),
				CEREAL_NVP(w));
		}
	};
}
