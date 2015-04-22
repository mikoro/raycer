// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Math/MathUtils.h"

#include <cmath>
#include <limits>
#include <algorithm>

using namespace Raycer;

namespace
{
	const double epsilon = std::numeric_limits<double>::epsilon();
}

bool MathUtils::almostZero(double value, int epsilons)
{
	return std::abs(value) < (epsilon * epsilons);
}

// https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
bool MathUtils::almostSame(double first, double second, int epsilons)
{
	double difference = std::abs(first - second);

	if (difference < (epsilon * epsilons))
		return true;

	double larger = std::max(std::abs(first), std::abs(second));

	if (difference < (larger * epsilon * epsilons))
		return true;

	return false;
}

double MathUtils::degToRad(double degrees)
{
	return (degrees * (M_PI / 180.0));
}

double MathUtils::radToDeg(double radians)
{
	return (radians * (180.0 / M_PI));
}

double MathUtils::smoothstep(double t)
{
	return t * t * (3 - 2 * t);
}

double MathUtils::smootherstep(double t)
{
	return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}
