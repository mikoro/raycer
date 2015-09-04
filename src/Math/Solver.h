// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <complex>
#include <functional>
#include <vector>

namespace Raycer
{
	struct QuadraticResult
	{
		int rootCount = 0;
		double roots[2];
	};

	class Solver
	{
	public:

		static double findRoot(std::function<double(double)> f, double begin, double end, int iterations = 32);
		static QuadraticResult findRoots(double a, double b, double c);
		static std::vector<std::complex<double>> findRoots(const std::vector<double>& coefficients, int iterations = 32);
	};
}
