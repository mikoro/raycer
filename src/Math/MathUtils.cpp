// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Math/MathUtils.h"

#include <cmath>
#include <limits>
#include <algorithm>

using namespace Raycer;

namespace
{
	const float epsilon = std::numeric_limits<float>::epsilon();
}

bool MathUtils::almostZero(float value, int epsilons)
{
	return std::abs(value) < (epsilon * epsilons);
}

// https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
bool MathUtils::almostSame(float first, float second, int epsilons)
{
	float difference = std::abs(first - second);

	if (difference < (epsilon * epsilons))
		return true;

	float larger = std::max(std::abs(first), std::abs(second));
	
	if (difference < (larger * epsilon * epsilons))
		return true;

	return false;
}

float MathUtils::degToRad(float degrees)
{
	return (degrees * ((float)M_PI / 180.0f));
}
