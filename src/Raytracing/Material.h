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

		double diffuseness = 1.0;
		double specularity = 1.0;
		double shininess = 1.0;
		double reflectivity = 0.0;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(id),
				CEREAL_NVP(textureId),
				CEREAL_NVP(diffuseness),
				CEREAL_NVP(specularity),
				CEREAL_NVP(shininess),
				CEREAL_NVP(reflectivity));
		}
	};
}
