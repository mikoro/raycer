// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/RandomSampler.h"
#include "Math/Vector2.h"

using namespace Raycer;

RandomSampler::RandomSampler()
{
	std::random_device rd;
	generator.seed(rd());
	randomOffset = std::uniform_real_distribution<double>(0.0, 1.0);
}

double RandomSampler::getSample1D(uint i, uint n)
{
	(void)i;
	(void)n;

	return randomOffset(generator);
}

Vector2 RandomSampler::getSample2D(uint ix, uint iy, uint nx, uint ny)
{
	(void)ix;
	(void)iy;
	(void)nx;
	(void)ny;

	return Vector2(randomOffset(generator), randomOffset(generator));
}
