// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	enum class WindowRunnerStates { None, Default };

	class WindowRunnerState
	{
	public:

		virtual ~WindowRunnerState()
		{
		}

		virtual void initialize() = 0;
		virtual void pause() = 0;
		virtual void resume() = 0;
		virtual void shutdown() = 0;

		virtual void update(double timeStep) = 0;
		virtual void render(double timeStep, double interpolation) = 0;

		virtual void windowResized(int width, int height) = 0;
		virtual void framebufferResized(int width, int height) = 0;
	};
}
