// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/CMJSampler.h"
#include "Math/Vector2.h"

using namespace Raycer;

namespace
{
	uint64_t permute(uint64_t i, uint64_t l, uint64_t p)
	{
		uint64_t w = l - 1;

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

double CMJSampler::getSample1D(uint64_t x, uint64_t n, uint64_t permutation, std::mt19937& generator)
{
	(void)x;
	(void)n;
	(void)permutation;
	(void)generator;

	assert(x < n);

	return 0.0;
}

Vector2 CMJSampler::getSample2D(uint64_t x, uint64_t y, uint64_t nx, uint64_t ny, uint64_t permutation, std::mt19937& generator)
{
	(void)permutation;

	assert(x < nx && y < ny);

	Vector2 result;
	std::uniform_real_distribution<double> randomOffset(0.0, 1.0);

	uint64_t sx = permute(x, nx, permutation * 0x68bc21eb);
	uint64_t sy = permute(y, ny, permutation * 0x02e5be93);

	result.x = (double(x) + (double(sy) + randomOffset(generator)) / double(ny)) / double(nx);
	result.y = (double(y) + (double(sx) + randomOffset(generator)) / double(nx)) / double(ny);

	return result;
}
