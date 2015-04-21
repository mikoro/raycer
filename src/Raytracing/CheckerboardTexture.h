// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Texture.h"
#include "Math/Color.h"

namespace Raycer
{
	class CheckerboardTexture : public Texture
	{
		public:

			void initialize();
			Color getColor(double u, double v) const;

			Color color1;
			Color color2;

			template<class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(id),
					CEREAL_NVP(color1),
					CEREAL_NVP(color2));
			}
	};
}
