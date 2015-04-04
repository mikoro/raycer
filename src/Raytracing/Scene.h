// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "Raytracing/Light.h"
#include "Raytracing/Shape.h"
#include "Raytracing/Camera.h"

namespace Raycer
{
	class Scene
	{
	public:

		std::vector<Light> lights;
		std::vector<Shape> shapes;
		Camera camera;
	};
}
