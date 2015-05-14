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
			double distance = 40.0;
			double steepness = 8.0;
			Color color;
	};
}
