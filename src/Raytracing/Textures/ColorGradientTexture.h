// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Raytracing/Textures/Texture.h"
#include "Utils/ColorGradient.h"

namespace Raycer
{
	class Color;
	class Vector2;
	class Vector3;

	class ColorGradientTexture : public Texture
	{
	public:

		void initialize() override;

		Color getColor(const Vector2& texcoord, const Vector3& position) const override;
		double getValue(const Vector2& texcoord, const Vector3& position) const override;
		Vector3 getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const override;

		bool hasHorizontalColorGradient = false;
		bool hasVerticalColorGradient = false;
		ColorGradient horizontalColorGradient;
		ColorGradient verticalColorGradient;
		double horizontalIntensity = 1.0;
		double verticalIntensity = 1.0;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("texture", cereal::base_class<Texture>(this)),
				CEREAL_NVP(hasHorizontalColorGradient),
				CEREAL_NVP(hasVerticalColorGradient),
				CEREAL_NVP(horizontalColorGradient),
				CEREAL_NVP(verticalColorGradient),
				CEREAL_NVP(horizontalIntensity),
				CEREAL_NVP(verticalIntensity));
		}
	};
}
