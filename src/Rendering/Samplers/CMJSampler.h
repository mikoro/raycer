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

	// Correlated Multi-Jittered Sampling
	class CMJSampler : public Sampler
	{
	public:

		CMJSampler();

		double getSample1D(uint x, uint n) override;
		Vector2 getSample2D(uint x, uint y, uint nx, uint ny) override;

	private:

		std::mt19937 generator;
		std::uniform_real_distribution<double> randomOffset;
	};
}
