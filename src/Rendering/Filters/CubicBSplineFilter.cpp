// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>

#include "Rendering/Filters/CubicBSplineFilter.h"
#include "Math/Vector2.h"

using namespace Raycer;

CubicBSplineFilter::CubicBSplineFilter(double B_, double C_)
{
	B = B_;
	C = C_;
}

double CubicBSplineFilter::getWeight(double x)
{
	x = std::abs(x) * 2.0;

	if (x < 1.0)
		return ((12.0 - 9.0 * B - 6.0 * C) * (x * x * x) + (-18.0 + 12.0 * B + 6.0 * C) * (x * x) + (6.0 - 2.0 * B)) * (1.0 / 6.0);
	else if (x <= 2.0)
		return ((-B - 6.0 * C) * (x * x * x) + (6.0 * B + 30.0 * C) * (x * x) + (-12.0 * B - 48.0 * C) * x + (8.0 * B + 24.0 * C)) * (1.0 / 6.0);
	else
		return 0.0;
}

double CubicBSplineFilter::getWeight(double x, double y)
{
	return getWeight(x) * getWeight(y);
}

double CubicBSplineFilter::getWeight(const Vector2& point)
{
	return getWeight(point.x) * getWeight(point.y);
}
