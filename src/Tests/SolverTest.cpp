// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#ifdef RUN_UNIT_TESTS

#include <cmath>

#include "catch/catch.hpp"

#include "Math/Solver.h"
#include "Math/MathUtils.h"

using namespace Raycer;

TEST_CASE("Solver functionality", "[solver]")
{
	auto f1 = [](double x) { return cos(x) - x * x * x; };
	double result = Solver::findRoot(f1, -2.0, 2.0, 32);

	REQUIRE(MathUtils::almostSame(result, 0.86547378345385151));

	std::vector<double> coefficients = { -1, -2, -3, -4 };
	auto result2 = Solver::findRoots(coefficients, 100);
}

#endif
