// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#ifdef RUN_UNIT_TESTS

#include <cmath>

#include "catch/catch.hpp"

#include "Math/Polynomial.h"
#include "Math/MathUtils.h"

using namespace Raycer;

TEST_CASE("Polynomial functionality", "[polynomial]")
{
	Polynomial polynomial;

	double coefficients[5] = { 2.0, 7.0, 5.0, 3.0, -10.0 };
	polynomial.setCoefficients(coefficients, 5);
	auto result1 = polynomial.findAllRoots(32);
	auto result2 = polynomial.findAllRealRoots(32, 0.0001);

	REQUIRE(result1.size() == 4);
	REQUIRE(result2.size() == 2);
}

#endif
