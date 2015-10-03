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

		double getSample(unsigned i, unsigned n, unsigned permutation = 0) override;
		Vector2 getSquareSample(unsigned ix, unsigned iy, unsigned nx, unsigned ny, unsigned permutation = 0) override;
		Vector2 getDiskSample(unsigned ix, unsigned iy, unsigned nx, unsigned ny, unsigned permutation = 0) override;
		Vector3 getHemisphereSample(const ONB& onb, double distribution, unsigned ix, unsigned iy, unsigned nx, unsigned ny, unsigned permutation = 0) override;

	private:

		std::mt19937 generator;
		std::uniform_real_distribution<double> randomDist;
	};
}
