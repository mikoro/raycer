// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/CheckerboardTexture.h"

using namespace Raycer;

void CheckerboardTexture::initialize()
{
}

Color CheckerboardTexture::getColor(double u, double v) const
{
	if (u < 0.5)
		return (v < 0.5) ? color1 : color2;
	else
		return (v < 0.5) ? color2 : color1;
}
