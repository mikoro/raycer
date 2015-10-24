// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/JitteredSampler.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

using namespace Raycer;

JitteredSampler::JitteredSampler()
{
	std::random_device rd;
	generator.seed(rd());
	randomOffset = std::uniform_real_distribution<double>(0.0, 1.0);
}

double JitteredSampler::getSample(uint i, uint n, uint permutation)
{
	(void)permutation;

	double x = (double(i) + randomOffset(generator)) / double(n);

	return x;
}

Vector2 JitteredSampler::getSquareSample(uint ix, uint iy, uint nx, uint ny, uint permutation)
{
	(void)permutation;

	Vector2 result;

	result.x = (double(ix) + randomOffset(generator)) / double(nx);
	result.y = (double(iy) + randomOffset(generator)) / double(ny);

	return result;
}
