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

	struct CubicResult
	{
		int rootCount = 0;
		double roots[3];
	};

	struct QuarticResult
	{
		int rootCount = 0;
		double roots[4];
	};

	class Solver
	{
	public:

		static QuadraticResult quadratic(double a, double b, double c);
		static CubicResult cubic(double a, double b, double c, double d);
		static QuarticResult quartic(double a, double b, double c, double d, double e);
	};
}
