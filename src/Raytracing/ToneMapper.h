// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

namespace Raycer
{
	class Color;

	enum class ToneMapType { GAMMA };

	class ToneMapper
	{
		public:

			Color apply(const Color& pixelColor);

			bool enabled = false;
			ToneMapType type = ToneMapType::GAMMA;
			double gamma = 1.0 / 2.2;

			template<class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(enabled),
					CEREAL_NVP(type),
					CEREAL_NVP(gamma));
			}
	};
}
