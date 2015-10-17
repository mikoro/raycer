// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <chrono>

#include "Math/MovingAverage.h"

namespace Raycer
{
	struct TracerState;

	class ConsoleRunner
	{
	public:

		int run();
		void run(TracerState& state);
		void interrupt();

		static void openImageExternally(const std::string& fileName);

	private:

		void printProgress(const std::chrono::time_point<std::chrono::high_resolution_clock>& startTime, size_t totalPixelCount, size_t pixelsProcessed);
		void printProgressOpenCL(const std::chrono::time_point<std::chrono::high_resolution_clock>& startTime);

		bool openCLInitialized = false;
		std::atomic<bool> interrupted;

		MovingAverage pixelsPerSecondAverage;
		MovingAverage remainingTimeAverage;

		uint openCLProgressCounter1 = 0;
		uint openCLProgressCounter2 = 0;
	};
}
