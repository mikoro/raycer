// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

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

		EulerAngle(float yaw = 0.0f, float pitch = 0.0f, float roll = 0.0f);
		EulerAngle(const EulerAngle& e);
		EulerAngle& operator=(const EulerAngle& e);

		Vector3 getDirectionVector() const;

		float yaw;
		float pitch;
		float roll;
	};
}
