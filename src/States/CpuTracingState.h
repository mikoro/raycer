// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

#include "States/InteractiveState.h"
#include "CpuRaytracing/Scene.h"

namespace Raycer
{
	class CpuTracingState : public InteractiveState
	{
	public:

		void initialize();
		void pause();
		void resume();
		void shutdown();

		void update(double timeStep);
		void render(double timeStep, double interpolation);

		void windowResized(int width, int height);
		void framebufferResized(int width, int height);

	private:

		Scene scene;

		std::atomic<bool> interrupted = false;
		std::atomic<int> pixelCount = 0;
		std::atomic<int> rayCount = 0;
	};
}
