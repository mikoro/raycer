// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/ColorTexture.h"

using namespace Raycer;

void ColorTexture::initialize()
{
}

Color ColorTexture::getColor(double u, double v) const
{
	(void)u;
	(void)v;

	return color;
}
