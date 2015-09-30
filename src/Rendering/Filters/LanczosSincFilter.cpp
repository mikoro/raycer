// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Filters/LanczosSincFilter.h"
#include "Math/Vector2.h"

using namespace Raycer;

namespace
{
	double sinc(double x)
	{
		return sin(M_PI * x) / (M_PI * x);
	}
}

LanczosSincFilter::LanczosSincFilter(double width_)
{
	width = width_;
}

double LanczosSincFilter::getWeight(double x)
{
	x = std::abs(x);

	if (x == 0.0)
		return 1.0;

	if (x > width)
		return 0.0;

	return sinc(x) * sinc(x / width);
}

double LanczosSincFilter::getWeight(double x, double y)
{
	return getWeight(x) * getWeight(y);
}

double LanczosSincFilter::getWeight(const Vector2& point)
{
	return getWeight(point.x) * getWeight(point.y);
}

double LanczosSincFilter::getWidth()
{
	return width;
}
