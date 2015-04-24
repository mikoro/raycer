// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

#include "InteractiveStates/InteractiveState.h"
#include "Raytracing/Raytracer.h"
#include "Raytracing/Scene.h"

namespace Raycer
{
	class RaytracingState : public InteractiveState
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

		RaytracerState state;
		Scene scene;

		std::atomic<bool> interrupted;
	};
}
