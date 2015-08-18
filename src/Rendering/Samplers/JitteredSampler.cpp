// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Rendering/Samplers/JitteredSampler.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

using namespace Raycer;

JitteredSampler::JitteredSampler()
{
	std::random_device rd;
	generator.seed(rd());
	randomDist = std::uniform_real_distribution<double>(0.0, 1.0);
}

double JitteredSampler::getSample1D(int i, int n, int permutation)
{
	(void)permutation;

	double x = ((double)i + randomDist(generator)) / (double)n;
	return x;
}

Vector2 JitteredSampler::getSample2D(int ix, int iy, int nx, int ny, int permutation)
{
	(void)permutation;

	Vector2 result;

	result.x = ((double)ix + randomDist(generator)) / (double)nx;
	result.y = ((double)iy + randomDist(generator)) / (double)ny;

	return result;
}

Vector2 JitteredSampler::getSampleDisk(int ix, int iy, int nx, int ny, int permutation)
{
	return Sampler::mapToDisk(getSample2D(ix, iy, nx, ny, permutation));
}

Vector3 JitteredSampler::getSampleHemisphere(const ONB& onb, double distribution, int ix, int iy, int nx, int ny, int permutation)
{
	return Sampler::mapToHemisphere(onb, distribution, getSample2D(ix, iy, nx, ny, permutation));
}
