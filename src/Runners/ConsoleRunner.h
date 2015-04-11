// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <chrono>

using namespace std::chrono;

namespace Raycer
{
	class ConsoleRunner
	{
	public:

		int run();

	private:

		void printProgress(const time_point<system_clock>& startTime, size_t totalPixelCount, size_t pixelCount, size_t rayCount);
	};
}
