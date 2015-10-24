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

double RandomSampler::getSample1D(uint64_t x, uint64_t n)
{
	(void)x;
	(void)n;

	return randomOffset(generator);
}

Vector2 RandomSampler::getSample2D(uint64_t x, uint64_t y, uint64_t nx, uint64_t ny)
{
	(void)x;
	(void)y;
	(void)nx;
	(void)ny;

	return Vector2(randomOffset(generator), randomOffset(generator));
}
