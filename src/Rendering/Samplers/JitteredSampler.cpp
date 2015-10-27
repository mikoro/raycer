// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/JitteredSampler.h"
#include "Math/Vector2.h"

using namespace Raycer;

double JitteredSampler::getSample1D(uint64_t x, uint64_t n, uint64_t permutation, std::mt19937& generator)
{
	(void)permutation;

	assert(x < n);

	std::uniform_real_distribution<double> randomOffset(0.0, 1.0);
	return (double(x) + randomOffset(generator)) / double(n);
}

Vector2 JitteredSampler::getSample2D(uint64_t x, uint64_t y, uint64_t nx, uint64_t ny, uint64_t permutation, std::mt19937& generator)
{
	(void)permutation;

	assert(x < nx && y < ny);

	Vector2 result;
	std::uniform_real_distribution<double> randomOffset(0.0, 1.0);

	result.x = (double(x) + randomOffset(generator)) / double(nx);
	result.y = (double(y) + randomOffset(generator)) / double(ny);

	return result;
}
