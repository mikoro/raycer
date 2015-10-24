// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/CMJSampler.h"
#include "Math/Vector2.h"

using namespace Raycer;

namespace
{
	uint permute(uint i, uint l, uint p)
	{
		uint w = l - 1;

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
	randomOffset = std::uniform_real_distribution<double>(0.0, 1.0);
}

double CMJSampler::getSample1D(uint i, uint n)
{
	(void)i;
	(void)n;

	return 0.0;
}

Vector2 CMJSampler::getSample2D(uint ix, uint iy, uint nx, uint ny)
{
	Vector2 result;

	uint sx = permute(ix, nx, permutation * 0x68bc21eb);
	uint sy = permute(iy, ny, permutation * 0x02e5be93);

	result.x = (double(ix) + (double(sy) + randomOffset(generator)) / double(ny)) / double(nx);
	result.y = (double(iy) + (double(sx) + randomOffset(generator)) / double(nx)) / double(ny);

	return result;
}
