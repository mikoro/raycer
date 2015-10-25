// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Filters/BellFilter.h"

using namespace Raycer;

namespace
{
	double calculateWeight(double s)
	{
		s = std::abs(s);

		if (s < 0.5)
			return 0.75 - (s * s);
		else if (s <= 1.5)
			return 0.5 * pow(s - 1.5, 2.0);
		else
			return 0.0;
	}
}

BellFilter::BellFilter()
{
	radiusX = 1.5;
	radiusY = 1.5;
}

double BellFilter::getWeightX(double x)
{
	return calculateWeight(x);
}

double BellFilter::getWeightY(double y)
{
	return calculateWeight(y);
}
