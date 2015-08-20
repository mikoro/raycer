// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Rendering/Samplers/Sampler.h"

namespace Raycer
{
	struct Light
	{
		Color color = Color::WHITE;
		double intensity = 0.0;
	};

	struct AmbientLight : public Light
	{
		bool enableOcclusion = false;
		double maxDistance = 10.0;
		SamplerType samplerType = SamplerType::CMJ;
		int samples = 3;
		double distribution = 1.0;
	};

	struct DirectionalLight : public Light
	{
		Vector3 direction;
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
	};

	struct SpotLight : public PointLight
	{
		Vector3 direction;
		double sideAttenuation = 1.0;
		double angle = 45.0;
	};
}
