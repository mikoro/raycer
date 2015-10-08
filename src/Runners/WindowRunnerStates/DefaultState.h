// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <memory>

#include "Runners/WindowRunnerStates/WindowRunnerState.h"
#include "Raytracing/Tracers/TracerState.h"
#include "Raytracing/Tracers/Tracer.h"
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

		void windowResized(size_t width, size_t height) override;
		void framebufferResized(size_t width, size_t height) override;

	private:

		TracerState state;
		Scene scene;
		std::unique_ptr<Tracer> tracer;

		std::atomic<bool> interrupted;
		uint currentTestSceneNumber = 1;
	};
}
