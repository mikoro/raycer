// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

namespace Raycer
{
	struct RaytraceInfo;
	struct Ray;
	class Scene;

	class CpuRaytracer
	{
	public:

		void trace(RaytraceInfo& info, std::atomic<bool>& interrupted);

	private:

		void shootRay(Ray& ray, const Scene& scene, std::atomic<bool>& interrupted, std::atomic<size_t>& raysProcessed);
	};
}
