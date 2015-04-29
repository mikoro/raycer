// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

#include "Raytracing/Textures/Texture.h"
#include "Rendering/Image.h"

namespace Raycer
{
	class Color;
	class Vector2;
	class Vector3;

	class ImageTexture : public Texture
	{
	public:

		void initialize();

		Color getColor(const Vector3& position, const Vector2& texcoord) const;
		double getValue(const Vector3& position, const Vector2& texcoord) const;

		std::string imageFilePath;

	private:

		Image image;
	};
}
