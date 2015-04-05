// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "catch/catch.hpp"

#include "Math/MathUtils.h"
#include "Math/EulerAngle.h"
#include "Math/Vector3.h"

using namespace Raycer;

TEST_CASE("EulerAngle functionality", "[eulerangle]")
{
	REQUIRE(EulerAngle(0.0, 0.0, 0.0).getDirectionVector() == Vector3(0.0, 0.0, -1.0));
	REQUIRE(EulerAngle(90.0, 0.0, 0.0).getDirectionVector() == Vector3(-1.0, 0.0, 0.0));
	REQUIRE(EulerAngle(-90.0, 0.0, 0.0).getDirectionVector() == Vector3(1.0, 0.0, 0.0));
	REQUIRE(EulerAngle(180.0, 0.0, 0.0).getDirectionVector() == Vector3(0.0, 0.0, 1.0));
	REQUIRE(EulerAngle(0.0, 90.0, 0.0).getDirectionVector() == Vector3(0.0, 1.0, 0.0));
	REQUIRE(EulerAngle(0.0, -90.0, 0.0).getDirectionVector() == Vector3(0.0, -1.0, 0.0));
	REQUIRE(EulerAngle(123.0, 90.0, 0.0).getDirectionVector() == Vector3(0.0, 1.0, 0.0));
}
