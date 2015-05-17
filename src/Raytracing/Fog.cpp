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

	double t1 = pixel.distance / distance;
	t1 = std::max(0.0, std::min(t1, 1.0));
	t1 = pow(t1, steepness);

	if (heightDispersion && pixel.position.y > 0.0)
	{
		double t2 = pixel.position.y / height;
		t2 = std::max(0.0, std::min(t2, 1.0));
		t2 = pow(t2, heightSteepness);
		t2 = 1.0 - t2;
		t1 *= t2;
	}

	return Color::lerp(pixel.color, color, t1);
}
