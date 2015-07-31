// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <chrono>
#include <atomic>

#include "Rendering/Image.h"
#include "Math/MovingAverage.h"

namespace Raycer
{
	struct RaytracerState;

	class ConsoleRunner
	{
	public:

		int run();
		void run(RaytracerState& state);
		void interrupt();

		Image& getResultImage();

	private:

		void printProgress(const std::chrono::time_point<std::chrono::high_resolution_clock>& startTime, int totalPixelCount, int pixelsProcessed, int raysProcessed);
		void printProgressOpenCL(const std::chrono::time_point<std::chrono::high_resolution_clock>& startTime);
		std::string humanizeNumberDecimal(double value);
		std::string humanizeNumberBytes(double value);

		bool openCLInitialized = false;
		std::atomic<bool> interrupted;
		Image image;

		MovingAverage pixelsPerSecondAverage;
		MovingAverage raysPerSecondAverage;
		MovingAverage remainingTimeAverage;

		int openCLProgressCounter1 = 0;
		int openCLProgressCounter2 = 0;
	};
}
