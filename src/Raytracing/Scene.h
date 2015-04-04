// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>
#include <memory>

#include "Raytracing/Light.h"
#include "Raytracing/Primitive.h"
#include "Raytracing/Camera.h"

namespace Raycer
{
	struct Scene
	{
		std::vector<std::shared_ptr<Primitive>> primitives;
		std::vector<Light> lights;

		Color ambientColor = Color::WHITE;

		Camera camera;
	};
}
