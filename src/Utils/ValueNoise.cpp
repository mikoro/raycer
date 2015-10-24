// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Utils/ValueNoise.h"

using namespace Raycer;

ValueNoise::ValueNoise()
{
	std::random_device rd;
	seed(rd());
}

ValueNoise::ValueNoise(uint32_t seed_)
{
	seed(seed_);
}

void ValueNoise::seed(uint32_t seed)
{
	m_seed = seed;
}

double ValueNoise::getNoise(double x, double y, double z) const
{
	// TODO: seems to work only with mt19937 - which is too slow for this
	std::mt19937 generator;
	std::uniform_real_distribution<double> randomOffset(0.0, 1.0);

	int64_t ix = int64_t(floor(x));
	int64_t iy = int64_t(floor(y));
	int64_t iz = int64_t(floor(z));

	double tx = x - ix;
	double ty = y - iy;
	double tz = z - iz;
	double txi = 1.0 - tx;
	double tyi = 1.0 - ty;
	double tzi = 1.0 - tz;

	// corners of a unit cube, cxyz
	generator.seed(getHashcode(ix, iy, iz));
	double c000 = randomOffset(generator);
	generator.seed(getHashcode(ix, iy, iz + 1));
	double c001 = randomOffset(generator);
	generator.seed(getHashcode(ix, iy + 1, iz));
	double c010 = randomOffset(generator);
	generator.seed(getHashcode(ix, iy + 1, iz + 1));
	double c011 = randomOffset(generator);
	generator.seed(getHashcode(ix + 1, iy, iz));
	double c100 = randomOffset(generator);
	generator.seed(getHashcode(ix + 1, iy, iz + 1));
	double c101 = randomOffset(generator);
	generator.seed(getHashcode(ix + 1, iy + 1, iz));
	double c110 = randomOffset(generator);
	generator.seed(getHashcode(ix + 1, iy + 1, iz + 1));
	double c111 = randomOffset(generator);

	// trilinear interpolation
	// interpolate in x-direction
	double c00 = c000 * txi + c100 * tx;
	double c01 = c001 * txi + c101 * tx;
	double c10 = c010 * txi + c110 * tx;
	double c11 = c011 * txi + c111 * tx;

	// interpolate in y-direction
	double c0 = c00 * tyi + c10 * ty;
	double c1 = c01 * tyi + c11 * ty;

	// interpolate in z-direction
	double c = c0 * tzi + c1 * tz;

	return std::max(0.0, std::min(c, 1.0));
}

double ValueNoise::getFbmNoise(uint64_t octaves, double lacunarity, double persistence, double x, double y, double z) const
{
	double result = 0.0;
	double frequency = 1.0;
	double amplitude = 1.0;

	for (uint64_t i = 0; i < octaves; ++i)
	{
		result += getNoise(x * frequency, y * frequency, z * frequency) * amplitude;
		frequency *= lacunarity;
		amplitude *= persistence;
	}

	return result;
}

uint32_t ValueNoise::getHashcode(int64_t x, int64_t y, int64_t z) const
{
	return uint32_t((m_seed * 16381) + (x * 17389) + (y * 18313) + (z * 19423));
}
