// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

/*

http://3dgep.com/understanding-quaternions/
q = w + xi + yj + zk

*/

namespace Raycer
{
	class Vector3;
	class AxisAngle;

	class Quaternion
	{
	public:

		Quaternion(double w = 0.0, double x = 0.0, double y = 0.0, double z = 0.0);
		Quaternion(const Quaternion& q);
		Quaternion& operator=(const Quaternion& q);
		Quaternion(const Vector3& axis, double angle);
		Quaternion(const AxisAngle& axisAngle);

		friend Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
		friend Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
		friend Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
		friend Vector3 operator*(const Quaternion& q, const Vector3& v);
		friend Vector3 operator*(const Vector3& v, const Quaternion& q);
		friend Quaternion operator*(const Quaternion& q, double s);
		friend Quaternion operator*(double s, const Quaternion& q);
		friend Quaternion operator/(const Quaternion& q, double s);
		friend Quaternion operator-(const Quaternion& q);

		friend bool operator==(const Quaternion& q1, const Quaternion& q2);
		friend bool operator!=(const Quaternion& q1, const Quaternion& q2);

		Quaternion& operator+=(const Quaternion& q);
		Quaternion& operator-=(const Quaternion& q);
		Quaternion& operator*=(const Quaternion& q);
		Quaternion& operator*=(double s);
		Quaternion& operator/=(double s);

		double length() const;
		double lengthSquared() const;
		void conjugate();
		Quaternion conjugated() const;
		void normalize();
		Quaternion normalized() const;
		bool isZero() const;
		bool isNormal() const;
		AxisAngle toAxisAngle() const;

		static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, double t);

		double w;
		double x;
		double y;
		double z;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(w),
				CEREAL_NVP(x),
				CEREAL_NVP(y),
				CEREAL_NVP(z));
		}
	};
}
