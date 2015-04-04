// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <limits>

#include "catch/catch.hpp"

#include "Math/MathUtils.h"

using namespace Raycer;

TEST_CASE("MathUtils functionality", "[mathutils]")
{
	const float epsilon = std::numeric_limits<float>::epsilon();
	const float min = std::numeric_limits<float>::min();

	REQUIRE(MathUtils::almostZero(0.0f, 1) == true);
	REQUIRE(MathUtils::almostZero(0.0001f, 1) == false);
	REQUIRE(MathUtils::almostZero(-0.0001f, 1) == false);
	REQUIRE(MathUtils::almostZero(epsilon / 2, 1) == true);
	REQUIRE(MathUtils::almostZero(epsilon, 2) == true);
	REQUIRE(MathUtils::almostZero(epsilon * 2, 1) == false);
	REQUIRE(MathUtils::almostZero(min, 1) == true);
	REQUIRE(MathUtils::almostZero(-epsilon / 2, 1) == true);
	REQUIRE(MathUtils::almostZero(-epsilon, 2) == true);
	REQUIRE(MathUtils::almostZero(-epsilon * 2, 1) == false);
	REQUIRE(MathUtils::almostZero(-min) == true);

	REQUIRE(MathUtils::almostSame(67329.234f, 67329.242f, 1) == true);
	REQUIRE(MathUtils::almostSame(67329.234f, 67329.250f, 2) == true);
	REQUIRE(MathUtils::almostSame(epsilon, epsilon, 1) == true);
	REQUIRE(MathUtils::almostSame(epsilon, epsilon * 2, 1) == false);
	REQUIRE(MathUtils::almostSame(epsilon, epsilon * 2, 10) == true);

	REQUIRE(MathUtils::almostSame(MathUtils::degToRad(90.0f), (float)M_PI / 2.0f));
	REQUIRE(MathUtils::almostSame(MathUtils::degToRad(180.0f), (float)M_PI));
}
