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

double RandomSampler::getSample1D(uint x, uint n)
{
	(void)x;
	(void)n;

	return randomOffset(generator);
}

Vector2 RandomSampler::getSample2D(uint x, uint y, uint nx, uint ny)
{
	(void)x;
	(void)y;
	(void)nx;
	(void)ny;

	return Vector2(randomOffset(generator), randomOffset(generator));
}
