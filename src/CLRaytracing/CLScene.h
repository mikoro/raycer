// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "CLRaytracing/CLStructs.h"

namespace Raycer
{
	class Scene;

	class CLScene
	{
	public:

		void readScene(const Scene& scene);

		Gpu::Info info;
		Gpu::Camera camera;
		std::vector<Gpu::Light> lights;
		std::vector<Gpu::Plane> planes;
		std::vector<Gpu::Sphere> spheres;
	};
}
