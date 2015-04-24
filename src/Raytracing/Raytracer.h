// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

namespace Raycer
{
	class RenderTarget;
	class Scene;
	struct Ray;

	struct RaytracerState
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

		void run(RaytracerState& state, std::atomic<bool>& interrupted);

	private:

		void traceRay(RaytracerState& state, Ray& ray, int& rayCount, std::atomic<bool>& interrupted);
	};
}
