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
		double maxDistance = 10.0;
		SamplerType samplerType = SamplerType::CMJ;
		int samples = 3;
		double distribution = 1.0;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("light", cereal::base_class<Light>(this)),
				CEREAL_NVP(enableOcclusion),
				CEREAL_NVP(maxDistance),
				CEREAL_NVP(samplerType),
				CEREAL_NVP(samples),
				CEREAL_NVP(distribution));
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
		double distance = 1000.0;
		double attenuation = 1.0;
		bool softShadows = false;
		double radius = 1.0;
		SamplerType samplerType = SamplerType::CMJ;
		int samples = 3;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("light", cereal::base_class<Light>(this)),
				CEREAL_NVP(position),
				CEREAL_NVP(distance),
				CEREAL_NVP(attenuation),
				CEREAL_NVP(softShadows),
				CEREAL_NVP(radius),
				CEREAL_NVP(samplerType),
				CEREAL_NVP(samples));
		}
	};

	struct SpotLight : public PointLight
	{
		Vector3 direction;
		double sideAttenuation = 1.0;
		double angle = 45.0;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("light", cereal::base_class<PointLight>(this)),
				CEREAL_NVP(direction),
				CEREAL_NVP(sideAttenuation),
				CEREAL_NVP(angle));
		}
	};
}
