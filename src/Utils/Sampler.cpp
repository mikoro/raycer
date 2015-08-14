// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Utils/Sampler.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

using namespace Raycer;

Sampler::Sampler()
{
	std::random_device rd;
	gen.seed(rd());
	realDist = std::uniform_real_distribution<double>(0.0, 1.0);
}

Vector2 Sampler::getRandomSample()
{
	return Vector2(realDist(gen), realDist(gen));
}

Vector2 Sampler::getRegularSample(int ix, int iy, int nx, int ny)
{
	double dx = ((double)ix + 0.5) / (double)nx;
	double dy = ((double)iy + 0.5) / (double)ny;

	return Vector2(dx, dy);
}

Vector2 Sampler::getJitteredSample(int ix, int iy, int nx, int ny)
{
	double dx = ((double)ix + realDist(gen)) / (double)nx;
	double dy = ((double)iy + realDist(gen)) / (double)ny;

	return Vector2(dx, dy);
}

double Sampler::getJitteredSample(int i, int n)
{
	return ((double)i + realDist(gen)) / (double)n;
}

namespace
{
	// Correlated Multi-Jittered Sampling (Andrew Kensler)
	unsigned int permute(unsigned int i, unsigned int l, unsigned int p)
	{
		unsigned int w = l - 1;

		w |= w >> 1;
		w |= w >> 2;
		w |= w >> 4;
		w |= w >> 8;
		w |= w >> 16;

		do
		{
			i ^= p;
			i *= 0xe170893d;
			i ^= p >> 16;
			i ^= (i & w) >> 4;
			i ^= p >> 8;
			i *= 0x0929eb3f;
			i ^= p >> 23;
			i ^= (i & w) >> 1;
			i *= 1 | p >> 27;
			i *= 0x6935fa69;
			i ^= (i & w) >> 11;
			i *= 0x74dcb303;
			i ^= (i & w) >> 2;
			i *= 0x9e501cc3;
			i ^= (i & w) >> 2;
			i *= 0xc860a3df;
			i &= w;
			i ^= i >> 5;
		} while (i >= l);

		return (i + p) % l;
	}
}

Vector2 Sampler::getCmjSample(int ix, int iy, int nx, int ny, int permutation)
{
	int sx = permute(ix, nx, permutation * 0x68bc21eb);
	int sy = permute(iy, ny, permutation * 0x02e5be93);

	double dx = ((double)ix + ((double)sy + realDist(gen)) / (double)ny) / (double)nx;
	double dy = ((double)iy + ((double)sx + realDist(gen)) / (double)nx) / (double)ny;

	return Vector2(dx, dy);
}

Vector2 Sampler::getCmjDiskSample(int ix, int iy, int nx, int ny, int permutation)
{
	int sx = permute(ix, nx, permutation * 0x68bc21eb);
	int sy = permute(iy, ny, permutation * 0x02e5be93);

	double dx = ((double)ix + ((double)sy + realDist(gen)) / (double)ny) / (double)nx;
	double dy = ((double)iy + ((double)sx + realDist(gen)) / (double)nx) / (double)ny;

	// square to disk polar mapping
	double r = sqrt(dx);
	double phi = 2.0 * M_PI * dy;
	dx = r * cos(phi);
	dy = r * sin(phi);

	return Vector2(dx, dy);
}

Vector3 Sampler::getCmjHemisphereSample(const Vector3& u, const Vector3& v, const Vector3& w, double distribution, int ix, int iy, int nx, int ny, int permutation)
{
	Vector2 sample = getCmjSample(ix, iy, nx, ny, permutation);

	double phi = 2.0 * M_PI * sample.x;
	double cos_phi = cos(phi);
	double sin_phi = sin(phi);
	double cos_theta = pow(1.0 - sample.y, 1.0 / (distribution + 1.0));
	double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

	double su = sin_theta * cos_phi;
	double sv = sin_theta * sin_phi;
	double sw = cos_theta;

	return su * u + sv * v + sw * w;
}

