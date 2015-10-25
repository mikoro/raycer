// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Filters/LanczosSincFilter.h"

using namespace Raycer;

namespace
{
	double sinc(double x)
	{
		return sin(M_PI * x) / (M_PI * x);
	}

	double calculateWeight(double s, double size)
	{
		s = std::abs(s);

		if (s == 0.0)
			return 1.0;

		if (s > size)
			return 0.0;

		return sinc(s) * sinc(s / size);
	}
}

LanczosSincFilter::LanczosSincFilter(uint64_t radiusX_, uint64_t radiusY_)
{
	setRadius(radiusX_, radiusY_);
}

void LanczosSincFilter::setRadius(uint64_t radiusX_, uint64_t radiusY_)
{
	radiusX = double(radiusX_);
	radiusY = double(radiusY_);
}

double LanczosSincFilter::getWeightX(double x)
{
	return calculateWeight(x, radiusX);
}

double LanczosSincFilter::getWeightY(double y)
{
	return calculateWeight(y, radiusY);
}
