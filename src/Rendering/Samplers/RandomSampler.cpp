// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/RandomSampler.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

using namespace Raycer;

RandomSampler::RandomSampler()
{
	std::random_device rd;
	generator.seed(rd());
	randomOffset = std::uniform_real_distribution<double>(0.0, 1.0);
}

double RandomSampler::getSample(uint i, uint n, uint permutation)
{
	(void)i;
	(void)n;
	(void)permutation;

	return randomOffset(generator);
}

Vector2 RandomSampler::getSquareSample(uint ix, uint iy, uint nx, uint ny, uint permutation)
{
	(void)ix;
	(void)iy;
	(void)nx;
	(void)ny;
	(void)permutation;

	return Vector2(randomOffset(generator), randomOffset(generator));
}
