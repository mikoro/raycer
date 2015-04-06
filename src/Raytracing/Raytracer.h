// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class RenderTarget;
	struct Scene;
	struct Ray;

	class Raytracer
	{
	public:

		static void traceFast(RenderTarget& renderTarget, const Scene& scene);
		static void shootRay(Ray& ray, const Scene& scene);

		static const int MAX_REFLECTIONS = 1;
	};
}
