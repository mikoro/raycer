// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cmath>

#include "Raytracing/Fog.h"

using namespace Raycer;

Color Fog::apply(const Color& pixelColor, double pixelDistance)
{
	double t = pixelDistance / distance;
	t = std::max(0.0, std::min(t, 1.0));
	t = pow(t, steepness);

	return Color::lerp(pixelColor, color, t);
}
