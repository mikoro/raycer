// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cmath>

#include "Math/Solver.h"

using namespace Raycer;

QuadraticResult Solver::quadratic(double a, double b, double c)
{
	QuadraticResult result;

	double discriminant = b * b - 4.0 * a * c;

	if (discriminant < 0.0)
		return result;

	double q = -0.5 * (b + copysign(1.0, b) * sqrt(discriminant));

	result.rootCount = 2;
	result.roots[0] = q / a;
	result.roots[1] = c / q;

	if (result.roots[0] > result.roots[1])
		std::swap(result.roots[0], result.roots[1]);

	return result;
}
