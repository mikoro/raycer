// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

#include "Runners/WindowRunnerStates/WindowRunnerState.h"
#include "Raytracing/RaytracerState.h"
#include "Raytracing/Scene.h"

namespace Raycer
{
	class DefaultState : public WindowRunnerState
	{
	public:

		void initialize() override;
		void pause() override;
		void resume() override;
		void shutdown() override;

		void update(double timeStep) override;
		void render(double timeStep, double interpolation) override;

		void windowResized(int width, int height) override;
		void framebufferResized(int width, int height) override;

	private:

		RaytracerState state;
		Scene scene;

		std::atomic<bool> interrupted;
		int currentTestSceneNumber = 1;
	};
}
