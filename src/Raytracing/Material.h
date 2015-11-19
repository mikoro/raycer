// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Rendering/Samplers/Sampler.h"

namespace Raycer
{
	class Texture;

	class Material
	{
	public:

		uint64_t id = 0;

		bool skipLighting = false;
		bool nonShadowing = false;
		bool normalInterpolation = false;
		bool invertNormal = false;
		bool enableCSG = false;

		Vector2 texcoordScale = Vector2(1.0, 1.0);

		Color ambientReflectance = Color(0.0, 0.0, 0.0);
		uint64_t ambientMapTextureId = 0;
		Texture* ambientMapTexture = nullptr;

		Color diffuseReflectance = Color(0.0, 0.0, 0.0);
		uint64_t diffuseMapTextureId = 0;
		Texture* diffuseMapTexture = nullptr;

		Color specularReflectance = Color(0.0, 0.0, 0.0);
		double specularShininess = 2.0;
		uint64_t specularMapTextureId = 0;
		Texture* specularMapTexture = nullptr;

		Color emittance = Color(0.0, 0.0, 0.0);
		uint64_t emittanceMapTextureId = 0;
		Texture* emittanceMapTexture = nullptr;

		bool fresnelReflection = false;
		double refractiveIndex = 1.0;

		double rayReflectance = 0.0;
		uint64_t rayReflectanceMapTextureId = 0;
		Texture* rayReflectanceMapTexture = nullptr;
		SamplerType rayReflectanceGlossinessSamplerType = SamplerType::CMJ;
		uint64_t rayReflectanceGlossinessSampleCountSqrt = 0;
		double rayReflectanceGlossiness = 1000.0;

		double rayTransmittance = 0.0;
		uint64_t rayTransmittanceMapTextureId = 0;
		Texture* rayTransmittanceMapTexture = nullptr;
		SamplerType rayTransmittanceGlossinessSamplerType = SamplerType::CMJ;
		uint64_t rayTransmittanceGlossinessSampleCountSqrt = 0;
		double rayTransmittanceGlossiness = 1000.0;
		bool enableRayTransmissionAttenuation = false;
		double rayTransmissionAttenuationFactor = 1.0;
		Color rayTransmissionAttenuationColor = Color(0.0, 0.0, 0.0);

		uint64_t normalMapTextureId = 0;
		Texture* normalMapTexture = nullptr;

		uint64_t maskMapTextureId = 0;
		Texture* maskMapTexture = nullptr;

		uint64_t heightMapTextureId = 0;
		Texture* heightMapTexture = nullptr;

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
				CEREAL_NVP(ambientReflectance),
				CEREAL_NVP(ambientMapTextureId),
				CEREAL_NVP(diffuseReflectance),
				CEREAL_NVP(diffuseMapTextureId),
				CEREAL_NVP(specularReflectance),
				CEREAL_NVP(specularShininess),
				CEREAL_NVP(specularMapTextureId),
				CEREAL_NVP(emittance),
				CEREAL_NVP(emittanceMapTextureId),
				CEREAL_NVP(fresnelReflection),
				CEREAL_NVP(refractiveIndex),
				CEREAL_NVP(rayReflectance),
				CEREAL_NVP(rayReflectanceMapTextureId),
				CEREAL_NVP(rayReflectanceGlossinessSamplerType),
				CEREAL_NVP(rayReflectanceGlossinessSampleCountSqrt),
				CEREAL_NVP(rayReflectanceGlossiness),
				CEREAL_NVP(rayTransmittance),
				CEREAL_NVP(rayTransmittanceMapTextureId),
				CEREAL_NVP(rayTransmittanceGlossinessSamplerType),
				CEREAL_NVP(rayTransmittanceGlossinessSampleCountSqrt),
				CEREAL_NVP(rayTransmittanceGlossiness),
				CEREAL_NVP(enableRayTransmissionAttenuation),
				CEREAL_NVP(rayTransmissionAttenuationFactor),
				CEREAL_NVP(rayTransmissionAttenuationColor),
				CEREAL_NVP(normalMapTextureId),
				CEREAL_NVP(maskMapTextureId),
				CEREAL_NVP(heightMapTextureId));
		}
	};
}
