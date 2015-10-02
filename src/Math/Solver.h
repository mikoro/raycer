// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <functional>

namespace Raycer
{
	struct QuadraticResult
	{
		unsigned rootCount = 0;
		double roots[2];
	};

	class Solver
	{
	public:

		static QuadraticResult findQuadraticRoots(double a, double b, double c);
		static double findRoot(const std::function<double(double)>& f, double begin, double end, unsigned iterations = 32);
	};
}
