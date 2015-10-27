// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/JitteredSampler.h"
#include "Math/Vector2.h"

using namespace Raycer;

JitteredSampler::JitteredSampler()
{
	std::random_device rd;
	generator.seed(rd());
	randomOffset = std::uniform_real_distribution<double>(0.0, 1.0);
}

double JitteredSampler::getSample1D(uint64_t x, uint64_t n, uint64_t permutation)
{
	(void)permutation;

	assert(x < n);

	return (double(x) + randomOffset(generator)) / double(n);
}

Vector2 JitteredSampler::getSample2D(uint64_t x, uint64_t y, uint64_t nx, uint64_t ny, uint64_t permutation)
{
	(void)permutation;

	assert(x < nx && y < ny);

	Vector2 result;

	result.x = (double(x) + randomOffset(generator)) / double(nx);
	result.y = (double(y) + randomOffset(generator)) / double(ny);

	return result;
}
