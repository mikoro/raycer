// Copyright � 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Texture.h"
#include "Utils/PerlinNoise.h"

namespace Raycer
{
	class Color;
	class Vector3;
	class Vector2;

	class MarbleTexture : public Texture
	{
		public:

			MarbleTexture();

			void initialize();
			Color getColor(const Vector3& position, const Vector2& texcoord) const;

			template<class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(id));
			}

		private:

			PerlinNoise perlinNoise;
	};
}
