// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

#include "Math/MovingAverage.h"

namespace Raycer
{
	class FpsCounter
	{
	public:

		FpsCounter();

		void count();
		void update(double timeStep);
		double getFps() const;
		std::string getFpsString() const;

	private:

		double lastTime = 0.0;
		double frameTime = 0.0;
		MovingAverage averageFrameTime;
	};
}
