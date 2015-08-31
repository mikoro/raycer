// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

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

		static QuadraticResult quadratic(double a, double b, double c);

		template<typename F>
		static double regulaFalsi(double begin, double end, F f, int maxIterations = 16);
	};
}
