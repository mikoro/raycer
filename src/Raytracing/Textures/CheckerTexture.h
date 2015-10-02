// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Raytracing/Textures/Texture.h"
#include "Math/Color.h"

namespace Raycer
{
	class Vector2;
	class Vector3;

	class CheckerTexture : public Texture
	{
	public:

		void initialize() override;

		Color getColor(const Vector2& texcoord, const Vector3& position) const;
		double getValue(const Vector2& texcoord, const Vector3& position) const override;
		Vector3 getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const override;

		Color color1;
		Color color2;
		bool stripeMode = false;
		double stripeWidth = 0.05;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("texture", cereal::base_class<Texture>(this)),
				CEREAL_NVP(color1),
				CEREAL_NVP(color2),
				CEREAL_NVP(stripeMode),
				CEREAL_NVP(stripeWidth));
		}
	};
}
