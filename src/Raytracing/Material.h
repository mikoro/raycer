// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Math/Color.h"
#include "Math/Vector2.h"

namespace Raycer
{
	class Texture;

	class Material
	{
	public:

		unsigned id = 0;

		bool skipLighting = false;
		bool nonShadowing = false;
		bool normalInterpolation = false;
		bool invertNormal = false;
		bool enableCSG = false;

		Vector2 texcoordScale = Vector2(1.0, 1.0);

		Texture* ambientMapTexture = nullptr;
		unsigned ambientMapTextureId = 0;
		Color ambientReflectance = Color(1.0, 1.0, 1.0);

		Texture* diffuseMapTexture = nullptr;
		unsigned diffuseMapTextureId = 0;
		Color diffuseReflectance = Color(1.0, 1.0, 1.0);

		Texture* specularMapTexture = nullptr;
		unsigned specularMapTextureId = 0;
		Color specularReflectance = Color(0.0, 0.0, 0.0);
		double shininess = 2.0;

		Texture* rayReflectanceMapTexture = nullptr;
		unsigned rayReflectanceMapTextureId = 0;
		double rayReflectance = 0.0;
		bool fresnelReflection = false;

		Texture* rayTransmittanceMapTexture = nullptr;
		unsigned rayTransmittanceMapTextureId = 0;
		double rayTransmittance = 0.0;
		double refractiveIndex = 1.0;
		bool enableAttenuation = false;
		double attenuationFactor = 1.0;
		Color attenuationColor = Color(0.0, 0.0, 0.0);

		Texture* normalMapTexture = nullptr;
		unsigned normalMapTextureId = 0;

		Texture* maskMapTexture = nullptr;
		unsigned maskMapTextureId = 0;

		Texture* heightMapTexture = nullptr;
		unsigned heightMapTextureId = 0;

	private:

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(id),
				CEREAL_NVP(skipLighting),
				CEREAL_NVP(nonShadowing),
				CEREAL_NVP(normalInterpolation),
				CEREAL_NVP(invertNormal),
				CEREAL_NVP(enableCSG),
				CEREAL_NVP(texcoordScale),
				CEREAL_NVP(ambientMapTextureId),
				CEREAL_NVP(ambientReflectance),
				CEREAL_NVP(diffuseMapTextureId),
				CEREAL_NVP(diffuseReflectance),
				CEREAL_NVP(specularMapTextureId),
				CEREAL_NVP(specularReflectance),
				CEREAL_NVP(shininess),
				CEREAL_NVP(rayReflectanceMapTextureId),
				CEREAL_NVP(rayReflectance),
				CEREAL_NVP(fresnelReflection),
				CEREAL_NVP(rayTransmittanceMapTextureId),
				CEREAL_NVP(rayTransmittance),
				CEREAL_NVP(refractiveIndex),
				CEREAL_NVP(enableAttenuation),
				CEREAL_NVP(attenuationFactor),
				CEREAL_NVP(attenuationColor),
				CEREAL_NVP(normalMapTextureId),
				CEREAL_NVP(maskMapTextureId),
				CEREAL_NVP(heightMapTextureId));
		}
	};
}
