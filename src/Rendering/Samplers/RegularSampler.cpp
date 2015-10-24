// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/RegularSampler.h"
#include "Math/Vector2.h"

using namespace Raycer;

double RegularSampler::getSample1D(uint x, uint n)
{
	return (double(x) + 0.5) / double(n);
}

Vector2 RegularSampler::getSample2D(uint x, uint y, uint nx, uint ny)
{
	Vector2 result;

	result.x = (double(x) + 0.5) / double(nx);
	result.y = (double(y) + 0.5) / double(ny);

	return result;
}
