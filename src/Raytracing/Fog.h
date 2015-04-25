// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Math/Color.h"

namespace Raycer
{
	class Vector3;

	class Fog
	{
		public:

			Color calculate(const Color& pixelColor, const Vector3& pixelPosition, double pixelDistance);

			bool enabled = false;
			double distance = 0.0;
			double steepness = 0.0;
			Color color = Color(0.0, 0.0, 0.0);

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
