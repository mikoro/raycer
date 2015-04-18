// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

/*

yaw = rotation around y-axis [0 1 0]
pitch = rotation around x-axis [1 0 0]
roll = rotation around z-axis [0 0 1]

when all zero, points towards negative z-axis [0 0 -1]

angles are in degrees

*/

namespace Raycer
{
	class Vector3;

	class EulerAngle
	{
	public:

		EulerAngle(double yaw = 0.0, double pitch = 0.0, double roll = 0.0);
		EulerAngle(const EulerAngle& e);
		EulerAngle& operator=(const EulerAngle& e);

		void clampPitch();
		void normalize();

		Vector3 getDirectionVector() const;

		static EulerAngle lerp(const EulerAngle& e1, const EulerAngle& e2, double t);

		double yaw;
		double pitch;
		double roll;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(yaw),
				CEREAL_NVP(pitch),
				CEREAL_NVP(roll));
		}
	};
}
