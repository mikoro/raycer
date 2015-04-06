// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>
#include <memory>

#include "Raytracing/Light.h"
#include "Raytracing/Primitive.h"
#include "Raytracing/Camera.h"
#include "Math/Color.h"

namespace Raycer
{
	struct Scene
	{
		std::vector<std::shared_ptr<Primitive>> primitives;
		std::vector<Light> lights;

		Camera camera;

		int maxReflections = 0;

		bool fogEnabled = false;
		double fogDistance = 0.0;
		double fogSteepness = 0.0;
		Color fogColor = Color(0.0, 0.0, 0.0);
	};
}
