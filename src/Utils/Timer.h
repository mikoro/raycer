// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <chrono>

#include "Math/MovingAverage.h"

namespace Raycer
{
	struct TimerData
	{
		uint64_t totalNanoseconds = 0;
		uint64_t totalMilliseconds = 0;
		uint64_t totalSeconds = 0;
		uint64_t totalMinutes = 0;
		uint64_t totalHours = 0;

		uint64_t hours = 0;
		uint64_t minutes = 0;
		uint64_t seconds = 0;
		uint64_t milliseconds = 0;

		std::string getString(bool withMilliseconds = false) const;
	};

	class Timer
	{
	public:

		Timer();

		void restart();
		TimerData getElapsed() const;
		
		void setTargetValue(double value);
		void updateCurrentValue(double value);
		void setAveragingAlpha(double alpha);
		TimerData getRemaining();
		double getPercentage() const;

	private:

		std::chrono::high_resolution_clock::time_point startTime;

		double currentValue = 0.0;
		double targetValue = 0.0;

		MovingAverage remainingMillisecondsAverage;
	};
}
