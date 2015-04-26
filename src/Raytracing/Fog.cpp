// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cmath>

#include "Raytracing/Fog.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Pixel.h"

using namespace Raycer;

Color Fog::calculate(const Scene& scene, const Pixel& pixel)
{
	(void)scene;

	double t = pixel.distance / distance;
	t = std::max(0.0, std::min(t, 1.0));
	t = pow(t, steepness);

	return Color::lerp(pixel.color, color, t);
}
