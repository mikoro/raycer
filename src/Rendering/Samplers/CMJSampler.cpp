// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Rendering/Samplers/CMJSampler.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

using namespace Raycer;

namespace
{
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

CMJSampler::CMJSampler()
{
	std::random_device rd;
	generator.seed(rd());
	randomDist = std::uniform_real_distribution<double>(0.0, 1.0);
}

double CMJSampler::getSample1D(int i, int n, int permutation)
{
	(void)i;
	(void)n;
	(void)permutation;

	return 0.0;
}

Vector2 CMJSampler::getSample2D(int ix, int iy, int nx, int ny, int permutation)
{
	Vector2 result;

	int sx = permute(ix, nx, permutation * 0x68bc21eb);
	int sy = permute(iy, ny, permutation * 0x02e5be93);

	result.x = ((double)ix + ((double)sy + randomDist(generator)) / (double)ny) / (double)nx;
	result.y = ((double)iy + ((double)sx + randomDist(generator)) / (double)nx) / (double)ny;

	return result;
}

Vector2 CMJSampler::getSampleDisk(int ix, int iy, int nx, int ny, int permutation)
{
	return Sampler::mapToDisk(getSample2D(ix, iy, nx, ny, permutation));
}

Vector3 CMJSampler::getSampleHemisphere(const ONB& onb, double distribution, int ix, int iy, int nx, int ny, int permutation)
{
	return Sampler::mapToHemisphere(onb, distribution, getSample2D(ix, iy, nx, ny, permutation));
}
