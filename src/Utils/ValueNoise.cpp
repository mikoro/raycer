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

ValueNoise::ValueNoise(int seed_)
{
	seed(seed_);
}

void ValueNoise::seed(int seed)
{
	m_seed = seed;
}

double ValueNoise::getNoise(double x, double y, double z) const
{
	// TODO: seems to work only with mt19937 - which is too slow for this
	std::mt19937 gen;
	std::uniform_real_distribution<double> realDist(0.0, 1.0);

	int ix = (int)floor(x);
	int iy = (int)floor(y);
	int iz = (int)floor(z);

	double tx = x - (double)ix;
	double ty = y - (double)iy;
	double tz = z - (double)iz;
	double txi = 1.0 - tx;
	double tyi = 1.0 - ty;
	double tzi = 1.0 - tz;

	// corners of a unit cube, cxyz
	gen.seed(getHashcode(ix, iy, iz));
	double c000 = realDist(gen);
	gen.seed(getHashcode(ix, iy, iz + 1));
	double c001 = realDist(gen);
	gen.seed(getHashcode(ix, iy + 1, iz));
	double c010 = realDist(gen);
	gen.seed(getHashcode(ix, iy + 1, iz + 1));
	double c011 = realDist(gen);
	gen.seed(getHashcode(ix + 1, iy, iz));
	double c100 = realDist(gen);
	gen.seed(getHashcode(ix + 1, iy, iz + 1));
	double c101 = realDist(gen);
	gen.seed(getHashcode(ix + 1, iy + 1, iz));
	double c110 = realDist(gen);
	gen.seed(getHashcode(ix + 1, iy + 1, iz + 1));
	double c111 = realDist(gen);

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

	return std::max(0.0, std::min(c, 1.0));;
}

double ValueNoise::getFbmNoise(int octaves, double lacunarity, double persistence, double x, double y, double z) const
{
	double result = 0.0;
	double frequency = 1.0;
	double amplitude = 1.0;

	for (int i = 0; i < octaves; ++i)
	{
		result += getNoise(x * frequency, y * frequency, z * frequency) * amplitude;
		frequency *= lacunarity;
		amplitude *= persistence;
	}

	return result;
}

int ValueNoise::getHashcode(int x, int y, int z) const
{
	return (m_seed * 16381) + (x * 17389) + (y * 18313) + (z * 19423);
}
