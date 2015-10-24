// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/RegularSampler.h"
#include "Math/Vector2.h"

using namespace Raycer;

double RegularSampler::getSample1D(uint64_t x, uint64_t n)
{
	return (double(x) + 0.5) / double(n);
}

Vector2 RegularSampler::getSample2D(uint64_t x, uint64_t y, uint64_t nx, uint64_t ny)
{
	Vector2 result;

	result.x = (double(x) + 0.5) / double(nx);
	result.y = (double(y) + 0.5) / double(ny);

	return result;
}
