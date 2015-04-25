// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

#include "cereal/cereal.hpp"

#include "Raytracing/Texture.h"
#include "Rendering/Image.h"

namespace Raycer
{
	class Color;
	class Vector3;
	class Vector2;

	class ImageTexture : public Texture
	{
		public:

			void initialize();
			Color getColor(const Vector3& position, const Vector2& texcoord) const;

			std::string imageFilePath;

			template<class Archive>
			void serialize(Archive& ar)
			{
				ar(CEREAL_NVP(id),
					CEREAL_NVP(imageFilePath));
			}

		private:

			Image image;
	};
}
