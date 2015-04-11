// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

#include "States/InteractiveState.h"
#include "CpuRaytracing/Scene.h"

namespace Raycer
{
	class GpuTracingState : public InteractiveState
	{
	public:

		void initialize();
		void pause();
		void resume();
		void shutdown();

		void update(double timeStep);
		void render(double timeStep, double interpolation);

		void windowResized(size_t width, size_t height);
		void framebufferResized(size_t width, size_t height);

	private:

		Scene scene;

		std::atomic<bool> interrupted = false;
		std::atomic<size_t> pixelCount = 0;
		std::atomic<size_t> rayCount = 0;
	};
}
