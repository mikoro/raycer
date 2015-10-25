﻿// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <random>

#include "Rendering/Samplers/Sampler.h"

namespace Raycer
{
	class RandomSampler : public Sampler
	{
	public:

		RandomSampler();

		double getSample1D(uint64_t x, uint64_t n) override;
		Vector2 getSample2D(uint64_t x, uint64_t y, uint64_t nx, uint64_t ny) override;

	private:

		std::mt19937 generator;
		std::uniform_real_distribution<double> randomOffset;
	};
}
