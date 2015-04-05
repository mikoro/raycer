// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class AppStateBase
	{
	public:

		virtual ~AppStateBase() {}

		virtual void initialize() = 0;
		virtual void pause() = 0;
		virtual void resume() = 0;
		virtual void shutdown() = 0;

		virtual void update(float timeStep) = 0;
		virtual void render(float timeStep, float interpolation) = 0;

		virtual void windowResized(int width, int height) = 0;
		virtual void framebufferResized(int width, int height) = 0;
	};
}
