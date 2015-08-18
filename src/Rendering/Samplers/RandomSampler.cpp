// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Rendering/Samplers/RandomSampler.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

using namespace Raycer;

RandomSampler::RandomSampler()
{
	std::random_device rd;
	generator.seed(rd());
	randomDist = std::uniform_real_distribution<double>(0.0, 1.0);
}

double RandomSampler::getSample1D(int i, int n, int permutation)
{
	(void)i;
	(void)n;
	(void)permutation;

	return randomDist(generator);
}

Vector2 RandomSampler::getSample2D(int ix, int iy, int nx, int ny, int permutation)
{
	(void)ix;
	(void)iy;
	(void)nx;
	(void)ny;
	(void)permutation;

	return Vector2(randomDist(generator), randomDist(generator));
}

Vector2 RandomSampler::getSampleDisk(int ix, int iy, int nx, int ny, int permutation)
{
	return Sampler::mapToDisk(getSample2D(ix, iy, nx, ny, permutation));
}

Vector3 RandomSampler::getSampleHemisphere(const ONB& onb, double distribution, int ix, int iy, int nx, int ny, int permutation)
{
	return Sampler::mapToHemisphere(onb, distribution, getSample2D(ix, iy, nx, ny, permutation));
}
