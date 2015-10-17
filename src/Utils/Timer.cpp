// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Utils/Timer.h"

using namespace Raycer;
namespace sc = std::chrono;

std::string TimerData::getString(bool withMilliseconds) const
{
	if (withMilliseconds)
		return tfm::format("%02d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);
	else
		return tfm::format("%02d:%02d:%02d", hours, minutes, seconds);
}

Timer::Timer(double targetValue_) : targetValue(targetValue_)
{
	restart();
}

void Timer::restart()
{
	startTime = sc::high_resolution_clock::now();
}

TimerData Timer::getElapsed() const
{
	auto elapsedTime = sc::high_resolution_clock::now() - startTime;

	TimerData timerData;

	timerData.totalNanoseconds = sc::duration_cast<sc::nanoseconds>(elapsedTime).count();
	timerData.totalMilliseconds = sc::duration_cast<sc::milliseconds>(elapsedTime).count();
	timerData.totalSeconds = sc::duration_cast<sc::seconds>(elapsedTime).count();
	timerData.totalMinutes = sc::duration_cast<sc::minutes>(elapsedTime).count();
	timerData.totalHours = sc::duration_cast<sc::hours>(elapsedTime).count();

	timerData.hours = timerData.totalHours;
	timerData.minutes = timerData.totalMinutes - timerData.hours * 60;
	timerData.seconds = timerData.totalSeconds - timerData.minutes * 60 - timerData.hours * 60 * 60;
	timerData.milliseconds = timerData.totalMilliseconds - timerData.seconds * 1000 - timerData.minutes * 60 * 1000 - timerData.hours * 60 * 60 * 1000;

	return timerData;
}

void Timer::setTargetValue(double value)
{
	targetValue = value;
}

void Timer::updateCurrentValue(double value)
{
	currentValue = value;
}

TimerData Timer::getRemaining() const
{
	auto elapsedTime = sc::high_resolution_clock::now() - startTime;
	uint64_t elapsedMilliseconds = sc::duration_cast<sc::milliseconds>(elapsedTime).count();
	double millisecondsPerUnit = 0.0;

	if (currentValue > 0.0)
		millisecondsPerUnit = double(elapsedMilliseconds) / currentValue;

	double remainingUnits = targetValue - currentValue;

	if (remainingUnits < 0.0)
		remainingUnits = 0.0;

	uint64_t remainingMilliseconds = uint64_t(remainingUnits * millisecondsPerUnit + 0.5);
	TimerData timerData;

	timerData.totalNanoseconds = remainingMilliseconds * 1000000;
	timerData.totalMilliseconds = remainingMilliseconds;
	timerData.totalSeconds = remainingMilliseconds / 1000;
	timerData.totalMinutes = remainingMilliseconds / 1000 / 60;
	timerData.totalHours = remainingMilliseconds / 1000 / 60 / 60;

	timerData.hours = timerData.totalHours;
	timerData.minutes = timerData.totalMinutes - timerData.hours * 60;
	timerData.seconds = timerData.totalSeconds - timerData.minutes * 60 - timerData.hours * 60 * 60;
	timerData.milliseconds = timerData.totalMilliseconds - timerData.seconds * 1000 - timerData.minutes * 60 * 1000 - timerData.hours * 60 * 60 * 1000;

	return timerData;
}
