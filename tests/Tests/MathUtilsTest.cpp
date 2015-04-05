// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <limits>

#include "catch/catch.hpp"

#include "Math/MathUtils.h"

using namespace Raycer;

TEST_CASE("MathUtils functionality", "[mathutils]")
{
	const double epsilon = std::numeric_limits<double>::epsilon();
	const double min = std::numeric_limits<double>::min();

	REQUIRE(MathUtils::almostZero(0.0, 1) == true);
	REQUIRE(MathUtils::almostZero(0.0001, 1) == false);
	REQUIRE(MathUtils::almostZero(-0.0001, 1) == false);
	REQUIRE(MathUtils::almostZero(epsilon / 2, 1) == true);
	REQUIRE(MathUtils::almostZero(epsilon, 2) == true);
	REQUIRE(MathUtils::almostZero(epsilon * 2, 1) == false);
	REQUIRE(MathUtils::almostZero(min, 1) == true);
	REQUIRE(MathUtils::almostZero(-epsilon / 2, 1) == true);
	REQUIRE(MathUtils::almostZero(-epsilon, 2) == true);
	REQUIRE(MathUtils::almostZero(-epsilon * 2, 1) == false);
	REQUIRE(MathUtils::almostZero(-min) == true);

	//REQUIRE(MathUtils::almostSame(67329.234, 67329.242, 1) == true);
	//REQUIRE(MathUtils::almostSame(67329.234, 67329.250, 2) == true);
	REQUIRE(MathUtils::almostSame(epsilon, epsilon, 1) == true);
	REQUIRE(MathUtils::almostSame(epsilon, epsilon * 2, 1) == false);
	REQUIRE(MathUtils::almostSame(epsilon, epsilon * 2, 10) == true);

	REQUIRE(MathUtils::almostSame(MathUtils::degToRad(90.0), M_PI / 2.0));
	REQUIRE(MathUtils::almostSame(MathUtils::degToRad(180.0), M_PI));
}
