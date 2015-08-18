// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Rendering/Samplers/Sampler.h"

namespace Raycer
{
	class RegularSampler : public Sampler
	{
	public:

		double getSample1D(int i, int n, int permutation = 0);
		Vector2 getSample2D(int ix, int iy, int nx, int ny, int permutation = 0);
		Vector2 getSampleDisk(int ix, int iy, int nx, int ny, int permutation = 0);
		Vector3 getSampleHemisphere(const ONB& onb, double distribution, int ix, int iy, int nx, int ny, int permutation = 0);
	};
}
