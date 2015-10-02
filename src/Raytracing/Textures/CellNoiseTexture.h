// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Raytracing/Textures/Texture.h"
#include "Utils/CellNoise.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

namespace Raycer
{
	class Vector2;

	class CellNoiseTexture : public Texture
	{
	public:

		void initialize() override;

		Color getColor(const Vector2& texcoord, const Vector3& position) const override;
		double getValue(const Vector2& texcoord, const Vector3& position) const override;
		Vector3 getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const override;

		int seed = 1;
		Vector3 scale = Vector3(10.0, 10.0, 10.0);
		Color baseColor = Color(1.0, 1.0, 1.0);
		CellNoiseDistanceType distanceType = CellNoiseDistanceType::EUCLIDEAN;
		CellNoiseCombineType combineType = CellNoiseCombineType::D1;
		int density = 1;

	private:

		CellNoise cellNoise;

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("texture", cereal::base_class<Texture>(this)),
				CEREAL_NVP(seed),
				CEREAL_NVP(scale),
				CEREAL_NVP(baseColor),
				CEREAL_NVP(distanceType),
				CEREAL_NVP(combineType),
				CEREAL_NVP(density));
		}
	};
}
