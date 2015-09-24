// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Raytracing/Textures/Texture.h"
#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Utils/ColorGradient.h"
#include "Math/EulerAngle.h"
#include "Utils/PerlinNoise.h"

namespace Raycer
{
	class Vector2;
	
	class AtmosphereTexture : public Texture
	{
	public:

		void initialize();

		Color getColor(const Vector2& texcoord, const Vector3& position) const;
		double getValue(const Vector2& texcoord, const Vector3& position) const;
		Vector3 getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const;

		int seed = 1;
		Color topColor1 = Color(0.0, 0.1, 0.6);
		Color topColor2 = Color(0.0, 0.2, 0.7);
		Color middleColor = Color(0.0, 0.4, 0.9);
		Color horizonColor = Color(0.3, 0.6, 0.9);
		Color cloudColor = Color(0.5, 0.5, 0.5);
		Color sunColor = Color(1.0, 1.0, 1.0);
		EulerAngle sunOrientation = EulerAngle(0.0, 0.0, 0.0);
		double sunSize = 1000.0;
		bool hasClouds = true;
		bool hasSun = true;

	private:

		ColorGradient gradient;
		PerlinNoise noise;
		Vector3 sunDirection;

		friend class cereal::access;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("texture", cereal::base_class<Texture>(this)),
				CEREAL_NVP(seed),
				CEREAL_NVP(topColor1),
				CEREAL_NVP(topColor2),
				CEREAL_NVP(middleColor),
				CEREAL_NVP(horizonColor),
				CEREAL_NVP(cloudColor),
				CEREAL_NVP(sunColor),
				CEREAL_NVP(sunOrientation),
				CEREAL_NVP(sunSize),
				CEREAL_NVP(hasClouds),
				CEREAL_NVP(hasSun));
		}
	};
}
