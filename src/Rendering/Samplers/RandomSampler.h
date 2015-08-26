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

	class RandomSampler : public Sampler
	{
	public:

		RandomSampler();

		double getSample(int i, int n, int permutation = 0);
		Vector2 getSquareSample(int ix, int iy, int nx, int ny, int permutation = 0);
		Vector2 getDiskSample(int ix, int iy, int nx, int ny, int permutation = 0);
		Vector3 getHemisphereSample(const ONB& onb, double distribution, int ix, int iy, int nx, int ny, int permutation = 0);

	private:

		std::mt19937 generator;
		std::uniform_real_distribution<double> randomDist;
	};
}
