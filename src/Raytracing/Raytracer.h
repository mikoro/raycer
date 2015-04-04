// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Framebuffer;
	struct Scene;

	class Raytracer
	{
	public:

		static void raytrace(const Framebuffer& framebuffer, const Scene& scene);
	};
}
