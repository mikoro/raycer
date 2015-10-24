// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/RegularSampler.h"
#include "Math/Vector2.h"

using namespace Raycer;

double RegularSampler::getSample1D(uint i, uint n)
{
	return (double(i) + 0.5) / double(n);
}

Vector2 RegularSampler::getSample2D(uint ix, uint iy, uint nx, uint ny)
{
	Vector2 result;

	result.x = (double(ix) + 0.5) / double(nx);
	result.y = (double(iy) + 0.5) / double(ny);

	return result;
}
