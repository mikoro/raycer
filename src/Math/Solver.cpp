// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Math/Solver.h"

using namespace Raycer;

// numerically stable quadratic formula
QuadraticResult Solver::findQuadraticRoots(double a, double b, double c)
{
	QuadraticResult result;

	double discriminant = b * b - 4.0 * a * c;

	if (discriminant < 0.0)
		return result;

	double q = -0.5 * (b + copysign(1.0, b) * sqrt(discriminant));

	result.roots[0] = q / a;
	result.roots[1] = c / q;
	result.rootCount = result.roots[0] == result.roots[1] ? 1 : 2;

	// order ascending
	if (result.roots[0] > result.roots[1])
		std::swap(result.roots[0], result.roots[1]);

	return result;
}

// false position / regula falsi
// https://en.wikipedia.org/wiki/False_position_method
double Solver::findRoot(const std::function<double(double)>& f, double begin, double end, int iterations)
{
	double x0 = 0.0;
	double y0 = 0.0;
	double x1 = begin;
	double y1 = f(x1);
	double x2 = end;
	double y2 = f(x2);

	for (int i = 0; i < iterations; ++i)
	{
		x0 = x1 - ((y1 * (x2 - x1)) / (y2 - y1));
		y0 = f(x0);

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
