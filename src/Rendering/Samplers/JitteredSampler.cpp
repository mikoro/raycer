// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Samplers/JitteredSampler.h"
#include "Math/Vector2.h"

using namespace Raycer;

JitteredSampler::JitteredSampler()
{
	std::random_device rd;
	generator.seed(rd());
	randomOffset = std::uniform_real_distribution<double>(0.0, 1.0);
}

double JitteredSampler::getSample1D(uint i, uint n)
{
	return (double(i) + randomOffset(generator)) / double(n);
}

Vector2 JitteredSampler::getSample2D(uint ix, uint iy, uint nx, uint ny)
{
	Vector2 result;

	result.x = (double(ix) + randomOffset(generator)) / double(nx);
	result.y = (double(iy) + randomOffset(generator)) / double(ny);

	return result;
}
