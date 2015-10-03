// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/RegularSampler.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

using namespace Raycer;

double RegularSampler::getSample(uint i, uint n, uint permutation)
{
	(void)permutation;

	double x = (double(i) + 0.5) / double(n);
	return x;
}

Vector2 RegularSampler::getSquareSample(uint ix, uint iy, uint nx, uint ny, uint permutation)
{
	(void)permutation;

	Vector2 result;

	result.x = (double(ix) + 0.5) / double(nx);
	result.y = (double(iy) + 0.5) / double(ny);

	return result;
}

Vector2 RegularSampler::getDiskSample(uint ix, uint iy, uint nx, uint ny, uint permutation)
{
	return Sampler::mapToDisk(getSquareSample(ix, iy, nx, ny, permutation));
}

Vector3 RegularSampler::getHemisphereSample(const ONB& onb, double distribution, uint ix, uint iy, uint nx, uint ny, uint permutation)
{
	return Sampler::mapToHemisphere(onb, distribution, getSquareSample(ix, iy, nx, ny, permutation));
}
