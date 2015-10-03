// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Raytracing/Textures/Texture.h"
#include "Utils/ValueNoise.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

namespace Raycer
{
	class Vector2;

	class ValueNoiseTexture : public Texture
	{
	public:

		void initialize() override;

		Color getColor(const Vector2& texcoord, const Vector3& position) const override;
		double getValue(const Vector2& texcoord, const Vector3& position) const override;
		Vector3 getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const override;

		uint seed = 1;
		Vector3 scale = Vector3(1.0, 1.0, 1.0);
		Color baseColor = Color(1.0, 1.0, 1.0);
		bool isFbm = false;
		uint octaves = 4;
		double lacunarity = 2.0;
		double persistence = 0.5;

	private:

		ValueNoise valueNoise;

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("texture", cereal::base_class<Texture>(this)),
				CEREAL_NVP(seed),
				CEREAL_NVP(scale),
				CEREAL_NVP(baseColor),
				CEREAL_NVP(isFbm),
				CEREAL_NVP(octaves),
				CEREAL_NVP(lacunarity),
				CEREAL_NVP(persistence));
		}
	};
}
