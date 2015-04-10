// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

namespace Raycer
{
	class RenderTarget;
	class Scene;
	struct Ray;

	class CpuRaytracer
	{
	public:

		static void trace(RenderTarget& renderTarget, const Scene& scene, std::atomic<bool>& interrupted, std::atomic<int>& pixelCount, std::atomic<int>& rayCount);

	private:

		static void shootRay(Ray& ray, const Scene& scene, std::atomic<bool>& interrupted, std::atomic<int>& rayCount);
	};
}
