// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <random>

#include "Rendering/Samplers/Sampler.h"

namespace Raycer
{
	class Vector2;
	class Vector3;
	class ONB;

	class JitteredSampler : public Sampler
	{
	public:

		JitteredSampler();

		double getSample(uint i, uint n, uint permutation = 0) override;
		Vector2 getSquareSample(uint ix, uint iy, uint nx, uint ny, uint permutation = 0) override;
		Vector2 getDiskSample(uint ix, uint iy, uint nx, uint ny, uint permutation = 0) override;
		Vector3 getHemisphereSample(const ONB& onb, double distribution, uint ix, uint iy, uint nx, uint ny, uint permutation = 0) override;

	private:

		std::mt19937 generator;
		std::uniform_real_distribution<double> randomDist;
	};
}
