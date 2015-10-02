// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Raytracing/Textures/Texture.h"
#include "Utils/PerlinNoise.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

namespace Raycer
{
	class Vector2;

	class PerlinNoiseTexture : public Texture
	{
	public:

		void initialize();

		Color getColor(const Vector2& texcoord, const Vector3& position) const;
		double getValue(const Vector2& texcoord, const Vector3& position) const;
		Vector3 getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const;

		int seed = 1;
		Vector3 scale = Vector3(10.0, 10.0, 10.0);
		Color baseColor = Color(1.0, 1.0, 1.0);
		bool isFbm = false;
		int octaves = 4;
		double lacunarity = 2.0;
		double persistence = 0.5;

	private:

		PerlinNoise perlinNoise;

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
