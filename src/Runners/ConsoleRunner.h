// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <memory>
#include <chrono>

#include "Runners/ConsoleSettings.h"

using namespace std::chrono;

namespace Raycer
{
	class BaseLog;
	class NamedLog;

	class ConsoleRunner
	{
	public:

		ConsoleRunner(BaseLog& baseLog);

		int run(const ConsoleSettings& settings);

	private:

		void printProgress(const time_point<system_clock>& startTime, int totalPixelCount, int pixelCount, int rayCount);

		std::unique_ptr<NamedLog> log;
	};
}
