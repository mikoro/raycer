// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Filters/BoxFilter.h"

using namespace Raycer;

BoxFilter::BoxFilter(double radiusX_, double radiusY_)
{
	setRadius(radiusX_, radiusY_);
}

void BoxFilter::setRadius(double radiusX_, double radiusY_)
{
	radiusX = radiusX_;
	radiusY = radiusY_;
	weightX = 1.0 / (2.0 * radiusX);
	weightY = 1.0 / (2.0 * radiusY);
}

double BoxFilter::getWeightX(double x)
{
	if (x >= -radiusX && x < radiusX)
		return weightX;
	else
		return 0.0;
}

double BoxFilter::getWeightY(double y)
{
	if (y >= -radiusY && y < radiusY)
		return weightY;
	else
		return 0.0;
}
