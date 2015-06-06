// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Utils/Sampler.h"
#include "Math/Vector2.h"

using namespace Raycer;

Sampler::Sampler()
{
	std::random_device rd;
	gen.seed(rd());
	realDist = std::uniform_real_distribution<double>(0.0, 1.0);
}

Vector2 Sampler::getCentroidSample(const Vector2& origin)
{
	return origin + Vector2(0.5, 0.5);
}

Vector2 Sampler::getRandomSample(const Vector2& origin)
{
	return origin + Vector2(realDist(gen), realDist(gen));
}

Vector2 Sampler::getRegularGridSample(const Vector2& origin, int ix, int iy, int nx, int ny)
{
	double dx = ((double)ix + 0.5) / (double)nx;
	double dy = ((double)iy + 0.5) / (double)ny;

	return origin + Vector2(dx, dy);
}

Vector2 Sampler::getJitteredSample(const Vector2& origin, int ix, int iy, int nx, int ny)
{
	double dx = ((double)ix + realDist(gen)) / (double)nx;
	double dy = ((double)iy + realDist(gen)) / (double)ny;

	return origin + Vector2(dx, dy);
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

Vector2 Sampler::getCmjSample(const Vector2& origin, int ix, int iy, int nx, int ny, int permutation)
{
	int sx = permute(ix, nx, permutation * 0x68bc21eb);
	int sy = permute(iy, ny, permutation * 0x02e5be93);

	double dx = ((double)ix + ((double)sy + realDist(gen)) / (double)ny) / (double)nx;
	double dy = ((double)iy + ((double)sx + realDist(gen)) / (double)nx) / (double)ny;

	return origin + Vector2(dx, dy);
}

Vector2 Sampler::getCmjDiskSample(const Vector2& origin, int ix, int iy, int nx, int ny, int permutation)
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
	dx = dx / 2.0 + 0.5;
	dy = dy / 2.0 + 0.5;

	return origin + Vector2(dx, dy);
}
