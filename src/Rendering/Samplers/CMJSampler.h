// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <random>

#include "Rendering/Samplers/Sampler.h"

namespace Raycer
{
	// Correlated Multi-Jittered Sampling
	class CMJSampler : public Sampler
	{
	public:

		CMJSampler();

		double getSample1D(uint64_t x, uint64_t n, uint64_t permutation = 0) override;
		Vector2 getSample2D(uint64_t x, uint64_t y, uint64_t nx, uint64_t ny, uint64_t permutation = 0) override;

	private:

		std::mt19937 generator;
		std::uniform_real_distribution<double> randomOffset;
	};
}
