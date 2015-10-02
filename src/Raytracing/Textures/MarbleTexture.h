// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Raytracing/Textures/Texture.h"
#include "Utils/PerlinNoise.h"
#include "Math/Color.h"

namespace Raycer
{
	class Vector2;
	class Vector3;

	class MarbleTexture : public Texture
	{
	public:

		void initialize() override;

		Color getColor(const Vector2& texcoord, const Vector3& position) const override;
		double getValue(const Vector2& texcoord, const Vector3& position) const override;
		Vector3 getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const override;

		unsigned seed = 1;
		Color marbleColor = Color(255, 252, 240);
		Color streakColor = Color(0, 33, 71);
		double density = 10.0;
		double swirliness = 15.0;
		double transparency = 2.0;

	private:

		PerlinNoise perlinNoise;

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("texture", cereal::base_class<Texture>(this)),
				CEREAL_NVP(seed),
				CEREAL_NVP(marbleColor),
				CEREAL_NVP(streakColor),
				CEREAL_NVP(density),
				CEREAL_NVP(swirliness),
				CEREAL_NVP(transparency));
		}
	};
}
