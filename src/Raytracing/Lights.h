// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Math/Vector3.h"
#include "Math/Color.h"

namespace Raycer
{
	struct Light
	{
		Color color = Color::WHITE;
		double intensity = 1.0;
	};

	struct AmbientLight : public Light
	{
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
	};

	struct SpotLight : public Light
	{
		Vector3 position;
		Vector3 direction;
		double distance = 1000.0;
		double distanceAttenuation = 1.0;
		double sideAttenuation = 1.0;
		double angle = 45.0;
	};
}
