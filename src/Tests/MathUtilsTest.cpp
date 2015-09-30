// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#ifdef RUN_UNIT_TESTS

#include "Math/MathUtils.h"

using namespace Raycer;

TEST_CASE("MathUtils functionality", "[mathutils]")
{
	const double epsilon = std::numeric_limits<double>::epsilon();
	const double min = std::numeric_limits<double>::min();

	REQUIRE(MathUtils::almostZero(0.0, epsilon) == true);
	REQUIRE(MathUtils::almostZero(0.0001, epsilon) == false);
	REQUIRE(MathUtils::almostZero(-0.0001, epsilon) == false);
	REQUIRE(MathUtils::almostZero(epsilon / 2, epsilon) == true);
	REQUIRE(MathUtils::almostZero(epsilon, 2 * epsilon) == true);
	REQUIRE(MathUtils::almostZero(epsilon * 2, epsilon) == false);
	REQUIRE(MathUtils::almostZero(min, epsilon) == true);
	REQUIRE(MathUtils::almostZero(-epsilon / 2, epsilon) == true);
	REQUIRE(MathUtils::almostZero(-epsilon, 2 * epsilon) == true);
	REQUIRE(MathUtils::almostZero(-epsilon * 2, epsilon) == false);
	REQUIRE(MathUtils::almostZero(-min) == true);

	REQUIRE(MathUtils::almostSame(epsilon, epsilon, epsilon) == true);
	REQUIRE(MathUtils::almostSame(epsilon, epsilon * 2, epsilon) == false);
	REQUIRE(MathUtils::almostSame(epsilon, epsilon * 2, 10 * epsilon) == true);

	REQUIRE(MathUtils::almostSame(MathUtils::degToRad(90.0), M_PI / 2.0));
	REQUIRE(MathUtils::almostSame(MathUtils::degToRad(180.0), M_PI));
}

#endif
