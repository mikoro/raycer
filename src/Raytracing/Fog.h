// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

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
			double distance = 0.0;
			double steepness = 0.0;
			Color color;

			template<class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(enabled),
					CEREAL_NVP(distance),
					CEREAL_NVP(steepness),
					CEREAL_NVP(color));
			}
	};
}
