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
	class AxisAngle;
	class Matrix4x4;
	class Vector3;

	class Quaternion
	{
	public:

		explicit Quaternion(double w = 0.0, double x = 0.0, double y = 0.0, double z = 0.0);
		explicit Quaternion(const AxisAngle& axisAngle);
		Quaternion(const Vector3& axis, double angle);

		friend Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
		friend Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
		friend Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
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

		Vector3 rotate(const Vector3& v) const;
		double length() const;
		double lengthSquared() const;
		void conjugate();
		Quaternion conjugated() const;
		void normalize();
		Quaternion normalized() const;
		bool isZero() const;
		bool isNan() const;
		bool isNormal() const;
		double dot(const Quaternion& q) const;
		AxisAngle toAxisAngle() const;
		Matrix4x4 toMatrix4x4() const;

		static Quaternion lerp(const Quaternion& q1, const Quaternion& q2, double t);
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
