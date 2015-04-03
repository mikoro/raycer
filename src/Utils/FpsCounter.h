// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

namespace Raycer
{
	class FpsCounter
	{
	public:

		FpsCounter();

		void countFrame();
		void update(double timeStep);
		double getFps() const;
		std::string getFpsString() const;

	private:

		double lastTime = 0;
		double frameTime = 0;
		double averageFrameTime = 1.0 / 30;
	};
}
