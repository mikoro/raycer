// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Color.h"

namespace Raycer
{
	struct Material
	{
		int id = 0;
		int textureId = 0;

		double ambientness = 1.0;
		double diffuseness = 1.0;
		double specularity = 1.0;
		double shininess = 1.0;
		double reflectance = 1.0;
		double transmittance = 1.0;
		double refractiveIndex = 1.0;
	};
}
