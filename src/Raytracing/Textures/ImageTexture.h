// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

#include "cereal/cereal.hpp"

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
		Vector3 getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const;

		const Image* getImage() const;
		int getImagePoolIndex() const;

		std::string imageFilePath;
		bool isBumpMap = false;
		bool isNormalMap = false;

	private:

		const Image* image;
		Image bumpMapX;
		Image bumpMapY;

		friend class cereal::access;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("texture", cereal::base_class<Texture>(this)),
				CEREAL_NVP(imageFilePath),
				CEREAL_NVP(isBumpMap),
				CEREAL_NVP(isNormalMap));
		}
	};
}
