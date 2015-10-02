// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Math/MathUtils.h"

using namespace Raycer;

bool MathUtils::almostZero(double value, double threshold)
{
	return std::abs(value) < threshold;
}

// https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
bool MathUtils::almostSame(double first, double second, double threshold)
{
	double difference = std::abs(first - second);

	if (difference < threshold)
		return true;

	double larger = std::max(std::abs(first), std::abs(second));

	if (difference < (larger * threshold))
		return true;

	return false;
}

bool MathUtils::almostSame(const std::complex<double>& first, const std::complex<double>& second, double threshold)
{
	return almostSame(first.real(), second.real(), threshold) && almostSame(first.imag(), second.imag(), threshold);
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

double MathUtils::fastPow(double a, double b)
{
	union
	{
		double d;
		int x[2];
	} u = {a};

	u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
	u.x[0] = 0;

	return u.d;
}
