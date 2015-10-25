// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Filters/GaussianFilter.h"

using namespace Raycer;

GaussianFilter::GaussianFilter(double stdDevX, double stdDevY)
{
	setStandardDeviations(stdDevX, stdDevY);
}

void GaussianFilter::setStandardDeviations(double stdDevX, double stdDevY)
{
	alphaX = 1.0 / (sqrt(2.0 * M_PI) * stdDevX);
	alphaY = 1.0 / (sqrt(2.0 * M_PI) * stdDevY);
	betaX = -1.0 / (2.0 * stdDevX * stdDevX);
	betaY = -1.0 / (2.0 * stdDevY * stdDevY);
	radiusX = (7.43384 * stdDevX) / 2.0; // from full width at thousandth of maximum
	radiusY = (7.43384 * stdDevY) / 2.0;
}

double GaussianFilter::getWeightX(double x)
{
	return alphaX * exp(x * x * betaX);
}

double GaussianFilter::getWeightY(double y)
{
	return alphaY * exp(y * y * betaY);
}
