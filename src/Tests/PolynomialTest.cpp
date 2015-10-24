// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#ifdef RUN_UNIT_TESTS

#include "catch/catch.hpp"

#include "Math/Polynomial.h"
#include "Math/MathUtils.h"

using namespace Raycer;

TEST_CASE("Polynomial functionality", "[polynomial]")
{
	Polynomial<5> polynomial;

	double coefficients[5] = { 2.0, 7.0, 5.0, 3.0, -10.0 };
	polynomial.setCoefficients(coefficients);

	uint64_t count;
	const double* result = polynomial.findAllPositiveRealRoots(count, 32, 0.0001);

	REQUIRE(count == 1);
	REQUIRE(MathUtils::almostSame(result[0], 0.79988784795406020));
}

#endif
