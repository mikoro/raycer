// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Rendering/Samplers/Sampler.h"

namespace Raycer
{
	struct Light
	{
		Color color = Color::WHITE;
		double intensity = 1.0;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(color),
				CEREAL_NVP(intensity));
		}
	};

	struct AmbientLight : public Light
	{
		bool enableAmbientOcclusion = false;
		SamplerType ambientOcclusionSamplerType = SamplerType::CMJ;
		uint64_t ambientOcclusionSampleCountSqrt = 0;
		double ambientOcclusionMaxSampleDistance = 1.0;
		double ambientOcclusionSampleDistribution = 1.0;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("light", cereal::base_class<Light>(this)),
				CEREAL_NVP(enableAmbientOcclusion),
				CEREAL_NVP(ambientOcclusionSamplerType),
				CEREAL_NVP(ambientOcclusionSampleCountSqrt),
				CEREAL_NVP(ambientOcclusionMaxSampleDistance),
				CEREAL_NVP(ambientOcclusionSampleDistribution));
		}
	};

	struct DirectionalLight : public Light
	{
		Vector3 direction;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("light", cereal::base_class<Light>(this)),
				CEREAL_NVP(direction));
		}
	};

	struct PointLight : public Light
	{
		Vector3 position;
		double maxDistance = 1000.0;
		double attenuation = 1.0;
		bool enableAreaLight = false;
		SamplerType areaLightSamplerType = SamplerType::CMJ;
		uint64_t areaLightSampleCountSqrt = 0;
		double areaLightRadius = 1.0;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("light", cereal::base_class<Light>(this)),
				CEREAL_NVP(position),
				CEREAL_NVP(maxDistance),
				CEREAL_NVP(attenuation),
				CEREAL_NVP(enableAreaLight),
				CEREAL_NVP(areaLightSamplerType),
				CEREAL_NVP(areaLightSampleCountSqrt),
				CEREAL_NVP(areaLightRadius));
		}
	};

	struct SpotLight : public PointLight
	{
		Vector3 direction;
		double angle = 45.0;
		double sideAttenuation = 1.0;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("light", cereal::base_class<PointLight>(this)),
				CEREAL_NVP(direction),
				CEREAL_NVP(angle),
				CEREAL_NVP(sideAttenuation));
		}
	};
}
