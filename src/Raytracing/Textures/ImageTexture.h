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

		Color getColor(const Vector2& texcoord, const Vector3& position) const;
		double getValue(const Vector2& texcoord, const Vector3& position) const;
		Vector3 getNormal(const Vector2& texcoord, const Vector3& position, TextureNormalType& type) const;

		std::string imageFilePath;
		bool applyGamma = false;
		double gamma = 2.2;
		bool isBumpMap = false;

	private:

		Image image;
		Image bumpMapX;
		Image bumpMapY;
	};
}
