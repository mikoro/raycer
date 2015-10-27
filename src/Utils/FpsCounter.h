// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/MovingAverage.h"

namespace Raycer
{
	class FpsCounter
	{
	public:

		FpsCounter();

		void tick();
		void update();
		double getFps() const;
		std::string getFpsString() const;

	private:

		double lastTime = 0.0;
		double frameTime = 0.0;
		MovingAverage averageFrameTime;
	};
}
