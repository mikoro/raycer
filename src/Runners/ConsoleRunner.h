// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

#include "Math/MovingAverage.h"
#include "Utils/Timer.h"

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

		void printProgress(const TimerData& elapsed, const TimerData& remaining);
		void printProgressOpenCL(const TimerData& elapsed, const TimerData& remaining);

		bool openCLInitialized = false;
		std::atomic<bool> interrupted;

		Timer timer;

		MovingAverage pixelsPerSecondAverage;

		uint progressCounter1 = 0;
		uint progressCounter2 = 0;
	};
}
