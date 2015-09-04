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

double Solver::falsePosition(std::function<double(double)> f, double begin, double end, double maxError, int maxIterations)
{
	double x0 = 0.0;
	double y0 = 0.0;
	double x1 = begin;
	double y1 = f(x1);
	double x2 = end;
	double y2 = f(x2);

	// no root
	if (y1 * y2 > 0.0)
		return begin;

	for (int i = 0; i < maxIterations; ++i)
	{
		x0 = x1 - ((y1 * (x2 - x1)) / (y2 - y1));
		y0 = f(x0);

		if (y0 < maxError)
			break;

		if (y0 * y1 > 0.0)
		{
			x1 = x0;
			y1 = y0;
		}
		else
		{
			x2 = x0;
			y2 = y0;
		}
	}

	return x0;
}
