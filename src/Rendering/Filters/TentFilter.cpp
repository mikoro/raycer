// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cmath>

#include "Rendering/Filters/TentFilter.h"
#include "Math/Vector2.h"

using namespace Raycer;

TentFilter::TentFilter(double width_)
{
	width = width_;
}

double TentFilter::getWeight(double x)
{
	return std::max(0.0, 1.0 - std::abs(x) / width);
}

double TentFilter::getWeight(double x, double y)
{
	return getWeight(x) * getWeight(y);
}

double TentFilter::getWeight(const Vector2& point)
{
	return getWeight(point.x) * getWeight(point.y);
}

double TentFilter::getWidth()
{
	return width;
}
