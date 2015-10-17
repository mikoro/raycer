// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "MovingAverage.h"

using namespace Raycer;

MovingAverage::MovingAverage(double alpha_, double average_)
{
	setAlpha(alpha_);
	setAverage(average_);
}

void MovingAverage::setAlpha(double alpha_)
{
	assert(alpha_ > 0.0 && alpha_ <= 1.0);

	alpha = alpha_;
}

void MovingAverage::setAverage(double average_)
{
	average = average_;
}

void MovingAverage::addMeasurement(double value)
{
	average = alpha * value + (1.0 - alpha) * average;
}

double MovingAverage::getAverage() const
{
	return average;
}
