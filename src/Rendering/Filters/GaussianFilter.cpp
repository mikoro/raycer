// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Filters/GaussianFilter.h"
#include "Math/Vector2.h"

using namespace Raycer;

GaussianFilter::GaussianFilter(double width_, double alpha_)
{
	width = width_;
	alpha = alpha_;
}

double GaussianFilter::getWeight(double x)
{
	x = std::abs(x);

	return std::max(0.0, exp(-alpha * x * x) - exp(-alpha * width * width));
}

double GaussianFilter::getWeight(double x, double y)
{
	return getWeight(x) * getWeight(y);
}

double GaussianFilter::getWeight(const Vector2& point)
{
	return getWeight(point.x) * getWeight(point.y);
}

double GaussianFilter::getWidth()
{
	return width;
}
