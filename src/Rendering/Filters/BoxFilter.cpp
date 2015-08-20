// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Rendering/Filters/BoxFilter.h"
#include "Math/Vector2.h"

using namespace Raycer;

double BoxFilter::getWeight(double x)
{
	if (std::abs(x) <= 1.0)
		return 1.0;
	else
		return 0.0;
}

double BoxFilter::getWeight(double x, double y)
{
	return getWeight(x) * getWeight(y);
}

double BoxFilter::getWeight(const Vector2& point)
{
	return getWeight(point.x) * getWeight(point.y);
}
