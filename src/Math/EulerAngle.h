// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

/*

pitch = rotation around x-axis [1 0 0]
yaw = rotation around y-axis [0 1 0]
roll = rotation around z-axis [0 0 1]

when all zero, direction points towards negative z-axis [0 0 -1]

angles are in degrees

*/

namespace Raycer
{
	class Vector3;

	class EulerAngle
	{
	public:

		EulerAngle(double pitch = 0.0, double yaw = 0.0, double roll = 0.0);
		EulerAngle(const EulerAngle& e);
		EulerAngle& operator=(const EulerAngle& e);

		friend EulerAngle operator+(const EulerAngle& e1, const EulerAngle& e2);
		friend EulerAngle operator-(const EulerAngle& e1, const EulerAngle& e2);
		friend EulerAngle operator*(const EulerAngle& e, double s);
		friend EulerAngle operator*(double s, const EulerAngle& e);
		friend EulerAngle operator-(const EulerAngle& e);

		EulerAngle& operator+=(const EulerAngle& e);
		EulerAngle& operator-=(const EulerAngle& e);

		void clampPitch();
		void normalize();

		Vector3 getDirection() const;

		static EulerAngle lerp(const EulerAngle& e1, const EulerAngle& e2, double t);

		double pitch;
		double yaw;
		double roll;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(pitch),
				CEREAL_NVP(yaw),
				CEREAL_NVP(roll));
		}
	};
}
