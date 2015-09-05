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
	QuadraticResult result1 = Solver::findQuadraticRoots(1.5, 4.0, -3.0);

	REQUIRE(result1.rootCount == 2);
	REQUIRE(MathUtils::almostSame(result1.roots[0], -3.2769839649484336));
	REQUIRE(MathUtils::almostSame(result1.roots[1], 0.61031729828176684));

	auto f1 = [](double x) { return cos(x) - x * x * x; };
	double result2 = Solver::findRoot(f1, -2.0, 2.0, 32);

	REQUIRE(MathUtils::almostSame(result2, 0.86547378345385151));
}

#endif
