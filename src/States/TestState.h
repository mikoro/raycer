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

	class TestState : public AppStateBase
	{
	public:

		TestState(BaseLog& baseLog, App& app, Framebuffer& framebuffer);

		void initialize();
		void pause();
		void resume();
		void shutdown();

		void update(double timeStep);
		void render(double timeStep, double interpolation);

	private:

		std::unique_ptr<NamedLog> log;

		App& app;
		Framebuffer& framebuffer;
		Scene scene;
	};
}
