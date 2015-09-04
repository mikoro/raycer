// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cassert>
#include <cmath>

#include "Math/Solver.h"

using namespace Raycer;

// false position / regula falsi
// https://en.wikipedia.org/wiki/False_position_method
double Solver::findRoot(std::function<double(double)> f, double begin, double end, int iterations)
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

// numerically stable quadratic formula
QuadraticResult Solver::findRoots(double a, double b, double c)
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

// Durand-Kerner algorithm
// https://en.wikipedia.org/wiki/Durand%E2%80%93Kerner_method
// Highest order coefficient is implicitly 1 and is not given.
// So the polynomial coefficients need to be normalized and start from n-1.
std::vector<std::complex<double>> Solver::findRoots(const std::vector<double>& coefficients, int iterations)
{
	int n = (int)coefficients.size();
	std::vector<std::complex<double>> roots(n);
	std::complex<double> seed(0.4, 0.9);

	// distribute start values
	for (int i = 0; i < n; ++i)
		roots[i] = pow(seed, i);

	// evaluate polynomial at x
	auto evaluate = [&](const std::complex<double>& x)
	{
		std::complex<double> y = pow(x, n);

		for (int i = 0; i < n; ++i)
			y += coefficients[i] * pow(x, n - i - 1);

		return y;
	};

	for (int i = 0; i < iterations; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			// numerator
			std::complex<double> temp = evaluate(roots[j]);

			// denominator
			for (int k = 0; k < n; ++k)
			{
				if (k == j)
					continue;

				temp /= (roots[j] - roots[k]);
			}

			// rest of the equation
			roots[j] = roots[j] - temp;
		}
	}

	return roots;
}
