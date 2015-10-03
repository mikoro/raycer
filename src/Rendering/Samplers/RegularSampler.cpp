// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/RegularSampler.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

using namespace Raycer;

double RegularSampler::getSample(unsigned i, unsigned n, unsigned permutation)
{
	(void)permutation;

	double x = (double(i) + 0.5) / double(n);
	return x;
}

Vector2 RegularSampler::getSquareSample(unsigned ix, unsigned iy, unsigned nx, unsigned ny, unsigned permutation)
{
	(void)permutation;

	Vector2 result;

	result.x = (double(ix) + 0.5) / double(nx);
	result.y = (double(iy) + 0.5) / double(ny);

	return result;
}

Vector2 RegularSampler::getDiskSample(unsigned ix, unsigned iy, unsigned nx, unsigned ny, unsigned permutation)
{
	return Sampler::mapToDisk(getSquareSample(ix, iy, nx, ny, permutation));
}

Vector3 RegularSampler::getHemisphereSample(const ONB& onb, double distribution, unsigned ix, unsigned iy, unsigned nx, unsigned ny, unsigned permutation)
{
	return Sampler::mapToHemisphere(onb, distribution, getSquareSample(ix, iy, nx, ny, permutation));
}
