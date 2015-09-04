// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#ifdef RUN_UNIT_TESTS

#include <cmath>

#include "catch/catch.hpp"

#include "Math/Solver.h"
#include "math/MathUtils.h"

using namespace Raycer;

TEST_CASE("Solver functionality", "[solver]")
{
	auto f1 = [](double x) { return cos(x) - x * x * x; };
	double result = Solver::falsePosition(f1, -2.0, 2.0, 0.000001, 32);

	REQUIRE(MathUtils::almostSame(result, 0.86547378345385151));
}

#endif
