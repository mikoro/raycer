// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Color.h"

namespace Raycer
{
	struct Material
	{
		Color color = Color::WHITE;

		double diffuseConstant = 1.0;
		double specularConstant = 1.0;
		double shininess = 1.0;
		double reflectivity = 0.0;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(color),
				CEREAL_NVP(diffuseConstant),
				CEREAL_NVP(specularConstant),
				CEREAL_NVP(shininess),
				CEREAL_NVP(reflectivity));
		}
	};
}
