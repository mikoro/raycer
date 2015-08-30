// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>

#include "Math/Solver.h"

using namespace Raycer;

QuadraticResult Solver::quadratic(double a, double b, double c)
{
	QuadraticResult result;

	double discriminant = b * b - 4.0 * a * c;

	if (discriminant < 0.0)
		return result;

	double discriminantSqrt = sqrt(discriminant);

	result.roots[0] = (-b + discriminantSqrt) / (2.0 * a);
	result.roots[1] = (-b - discriminantSqrt) / (2.0 * a);
	result.rootCount = (discriminant == 0.0) ? 1 : 2;

	if (result.roots[0] > result.roots[1])
		std::swap(result.roots[0], result.roots[1]);

	return result;
}

CubicResult Solver::cubic(double a, double b, double c, double d)
{
	(void)a;
	(void)b;
	(void)c;
	(void)d;

	return CubicResult();
}

QuarticResult Solver::quartic(double a, double b, double c, double d, double e)
{
	(void)a;
	(void)b;
	(void)c;
	(void)d;
	(void)e;

	return QuarticResult();
}
