// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <memory>
#include <atomic>

#include "States/InteractiveState.h"
#include "CpuRaytracing/Scene.h"

namespace Raycer
{
	class BaseLog;
	class NamedLog;
	class InteractiveRunner;
	class Framebuffer;
	struct InteractiveSettings;

	class TraceFastState : public InteractiveState
	{
	public:

		TraceFastState(BaseLog& baseLog, InteractiveRunner& runner, Framebuffer& framebuffer, InteractiveSettings& settings);

		void initialize();
		void pause();
		void resume();
		void shutdown();

		void update(double timeStep);
		void render(double timeStep, double interpolation);

		void windowResized(int width, int height);
		void framebufferResized(int width, int height);

	private:

		std::unique_ptr<NamedLog> log;

		InteractiveRunner& runner;
		Framebuffer& framebuffer;
		InteractiveSettings& settings;
		Scene scene;

		std::atomic<bool> interrupted = false;
		std::atomic<int> pixelCount = 0;
		std::atomic<int> rayCount = 0;
	};
}
