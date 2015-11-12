// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

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

		void initialize() override;

		Color getColor(const Vector2& texcoord, const Vector3& position) const override;
		double getValue(const Vector2& texcoord, const Vector3& position) const override;
		Vector3 getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const override;

		const Image* getImage() const;
		uint64_t getImagePoolIndex() const;

		std::string imageFilePath;
		bool isBumpMap = false;
		bool isNormalMap = false;
		bool applyGamma = false;

	private:

		const Image* image = nullptr;
		Image bumpMapX;
		Image bumpMapY;

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("texture", cereal::base_class<Texture>(this)),
				CEREAL_NVP(imageFilePath),
				CEREAL_NVP(isBumpMap),
				CEREAL_NVP(isNormalMap),
				CEREAL_NVP(applyGamma));
		}
	};
}
