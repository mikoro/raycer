// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

namespace Raycer
{
	class RenderTarget;
	class Scene;
	struct Ray;

	class Raytracer
	{
	public:

		static void traceFast(RenderTarget& renderTarget, const Scene& scene, std::atomic<bool>& interrupted, std::atomic<int>& pixelCount, std::atomic<int>& rayCount);
		static void traceFull(RenderTarget& renderTarget, const Scene& scene, std::atomic<bool>& interrupted, std::atomic<int>& pixelCount, std::atomic<int>& rayCount);

	private:

		static void shootRay(Ray& ray, const Scene& scene, std::atomic<bool>& interrupted, std::atomic<int>& rayCount);
	};
}
