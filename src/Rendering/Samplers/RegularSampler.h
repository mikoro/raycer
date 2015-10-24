﻿// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Rendering/Samplers/Sampler.h"

namespace Raycer
{
	class Vector2;
	class Vector3;
	class ONB;

	class RegularSampler : public Sampler
	{
	public:

		double getSample1D(uint i, uint n) override;
		Vector2 getSample2D(uint ix, uint iy, uint nx, uint ny) override;
	};
}
