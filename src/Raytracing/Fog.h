// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Color.h"

namespace Raycer
{
	class Scene;
	struct Pixel;

	class Fog
	{
		public:

			Color calculate(const Scene& scene, const Pixel& pixel);

			bool enabled = false;
			Color color;
			double distance = 0.0;
			double steepness = 1.0;
			bool heightDispersion = false;
			double height = 0.0;
			double heightSteepness = 1.0;
	};
}
