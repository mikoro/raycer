// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

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

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(color),
				CEREAL_NVP(intensity));
		}
	};

	struct AmbientLight : public Light
	{
		bool enableOcclusion = false;
		double maxOcclusionDistance = 1.0;
		SamplerType occlusionSamplerType = SamplerType::CMJ;
		int occlusionSamples = 0;
		double occlusionSampleDistribution = 1.0;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("light", cereal::base_class<Light>(this)),
				CEREAL_NVP(enableOcclusion),
				CEREAL_NVP(maxOcclusionDistance),
				CEREAL_NVP(occlusionSamplerType),
				CEREAL_NVP(occlusionSamples),
				CEREAL_NVP(occlusionSampleDistribution));
		}
	};

	struct DirectionalLight : public Light
	{
		Vector3 direction;

		template<class Archive>
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
		double radius = 1.0;
		bool enableSoftShadows = false;
		SamplerType softShadowSamplerType = SamplerType::CMJ;
		int softShadowSamples = 0;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("light", cereal::base_class<Light>(this)),
				CEREAL_NVP(position),
				CEREAL_NVP(maxDistance),
				CEREAL_NVP(attenuation),
				CEREAL_NVP(radius),
				CEREAL_NVP(enableSoftShadows),
				CEREAL_NVP(softShadowSamplerType),
				CEREAL_NVP(softShadowSamples));
		}
	};

	struct SpotLight : public PointLight
	{
		Vector3 direction;
		double angle = 45.0;
		double sideAttenuation = 1.0;
		
		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("light", cereal::base_class<PointLight>(this)),
				CEREAL_NVP(direction),
				CEREAL_NVP(angle),
				CEREAL_NVP(sideAttenuation));
		}
	};
}
