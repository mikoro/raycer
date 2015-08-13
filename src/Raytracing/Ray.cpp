// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Ray.h"

using namespace Raycer;

Ray::Ray(const Vector3& origin_, const Vector3& direction_)
{
	origin = origin_;
	direction = direction_;

	inverseDirection = direction.inversed();

	sign[0] = (direction.x < 0.0);
	sign[1] = (direction.y < 0.0);
	sign[2] = (direction.z < 0.0);
}
