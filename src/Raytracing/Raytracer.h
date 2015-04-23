// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

namespace Raycer
{
	class RenderTarget;
	class Scene;
	struct Ray;

	struct RaytracerConfig
	{
		RenderTarget* renderTarget = nullptr;
		Scene* scene = nullptr;

		int sceneWidth = 0;
		int sceneHeight = 0;
		int pixelOffset = 0;
		int pixelCount = 0;

		bool isInteractive = false;

		std::atomic<int> pixelsProcessed;
		std::atomic<int> raysProcessed;
	};

	class Raytracer
	{
	public:

		void trace(RaytracerConfig& config, std::atomic<bool>& interrupted);

	private:

		void shootRay(RaytracerConfig& config, Ray& ray, int& rayCount, std::atomic<bool>& interrupted);
	};
}
