// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <memory>

#include "App/AppStateBase.h"
#include "Raytracing/Scene.h"

namespace Raycer
{
	class App;
	class BaseLog;
	class NamedLog;
	class Framebuffer;
	struct Settings;

	class TraceFastState : public AppStateBase
	{
	public:

		TraceFastState(BaseLog& baseLog, App& app, Framebuffer& framebuffer, Settings& settings);

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

		App& app;
		Framebuffer& framebuffer;
		Settings& settings;
		Scene scene;
	};
}
