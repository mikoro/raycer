// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Ray.h"

using namespace Raycer;

Ray::Ray(const Vector3& origin_, const Vector3& direction_) : origin(origin_), direction(direction_)
{
}
