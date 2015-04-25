// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Raytracing/Texture.h"
#include "Math/Color.h"

namespace Raycer
{
	class Vector3;
	class Vector2;

	class ColorTexture : public Texture
	{
		public:

			void initialize();
			Color getColor(const Vector3& position, const Vector2& texcoord) const;

			Color color;

			template<class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(id),
					CEREAL_NVP(color));
			}
	};
}
