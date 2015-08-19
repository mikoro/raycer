// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>

#include "Rendering/Filters/BellFilter.h"
#include "Math/Vector2.h"

using namespace Raycer;

double BellFilter::getWeight(double x)
{
	x = abs(x) * 1.5;

	if (x < 0.5)
		return 0.75 - (x * x);
	else if (x <= 1.5)
		return 0.5 * pow(x - 1.5, 2.0);
	else
		return 0.0;
}

double BellFilter::getWeight(double x, double y)
{
	return getWeight(x) * getWeight(y);
}

double BellFilter::getWeight(const Vector2& point)
{
	return getWeight(point.x) * getWeight(point.y);
}
