// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/CMJSampler.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

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
	randomDist = std::uniform_real_distribution<double>(0.0, 1.0);
}

double CMJSampler::getSample(uint i, uint n, uint permutation)
{
	(void)i;
	(void)n;
	(void)permutation;

	return 0.0;
}

Vector2 CMJSampler::getSquareSample(uint ix, uint iy, uint nx, uint ny, uint permutation)
{
	Vector2 result;

	uint sx = permute(ix, nx, permutation * 0x68bc21eb);
	uint sy = permute(iy, ny, permutation * 0x02e5be93);

	result.x = (double(ix) + (double(sy) + randomDist(generator)) / double(ny)) / double(nx);
	result.y = (double(iy) + (double(sx) + randomDist(generator)) / double(nx)) / double(ny);

	return result;
}

Vector2 CMJSampler::getDiskSample(uint ix, uint iy, uint nx, uint ny, uint permutation)
{
	return Sampler::mapToDisk(getSquareSample(ix, iy, nx, ny, permutation));
}

Vector3 CMJSampler::getHemisphereSample(const ONB& onb, double distribution, uint ix, uint iy, uint nx, uint ny, uint permutation)
{
	return Sampler::mapToHemisphere(onb, distribution, getSquareSample(ix, iy, nx, ny, permutation));
}
