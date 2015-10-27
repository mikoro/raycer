// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/PoissonDiscSampler.h"
#include "Math/Vector2.h"
#include "Utils/PoissonDisc.h"

using namespace Raycer;

double PoissonDiscSampler::getSample1D(uint64_t x, uint64_t n, uint64_t permutation, std::mt19937& generator)
{
	(void)x;
	(void)n;
	(void)permutation;
	(void)generator;

	assert(x < n);

	return 0.0;
}

Vector2 PoissonDiscSampler::getSample2D(uint64_t x, uint64_t y, uint64_t nx, uint64_t ny, uint64_t permutation, std::mt19937& generator)
{
	(void)x;
	(void)y;
	(void)nx;
	(void)ny;
	(void)permutation;
	(void)generator;

	assert(x < nx && y < ny);

	return Vector2();
}

void PoissonDiscSampler::generateSamples2D(uint64_t sampleCountSqrt, std::mt19937& generator)
{
	(void)generator;

	PoissonDisc poissonDisc;
	samples2D = poissonDisc.generate(sampleCountSqrt, sampleCountSqrt, 1.0 / M_SQRT2, 30, true); // minDistance is just a guess to get about sampleCountSqrt^2 samples
}
