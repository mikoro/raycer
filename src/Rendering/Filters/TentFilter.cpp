// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Filters/TentFilter.h"

using namespace Raycer;

namespace
{
	double calculateWeight(double s)
	{
		s = std::abs(s);

		if (s < 1.0)
			return 1.0 - s;
		else
			return 0.0;
	}
}

TentFilter::TentFilter(double radiusX_, double radiusY_)
{
	setRadius(radiusX_, radiusY_);
}

void TentFilter::setRadius(double radiusX_, double radiusY_)
{
	radiusX = radiusX_;
	radiusY = radiusY_;
	radiusXInv = 1.0 / radiusX;
	radiusYInv = 1.0 / radiusY;
}

double TentFilter::getWeightX(double x)
{
	return calculateWeight(x * radiusXInv) * radiusXInv;
}

double TentFilter::getWeightY(double y)
{
	return calculateWeight(y * radiusYInv) * radiusYInv;
}
