// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Ray.h"

using namespace Raycer;

void Ray::update()
{
	inverseDirection = direction.inversed();
}
