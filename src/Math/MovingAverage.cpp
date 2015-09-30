// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "MovingAverage.h"

using namespace Raycer;

MovingAverage::MovingAverage(double alpha_, double averageValue_) : alpha(alpha_), averageValue(averageValue_)
{
}

void MovingAverage::setAlpha(double value)
{
	alpha = value;
}

void MovingAverage::setAverage(double value)
{
	averageValue = value;
}

void MovingAverage::addMeasurement(double value)
{
	averageValue = alpha * value + (1.0 - alpha) * averageValue;
}

double MovingAverage::getAverage() const
{
	return averageValue;
}
