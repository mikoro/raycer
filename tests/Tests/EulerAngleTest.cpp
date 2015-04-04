// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "catch/catch.hpp"

#include "Math/MathUtils.h"
#include "Math/EulerAngle.h"
#include "Math/Vector3.h"

using namespace Raycer;

TEST_CASE("EulerAngle functionality", "[eulerangle]")
{
	REQUIRE(EulerAngle(0.0f, 0.0f, 0.0f).getDirectionVector() == Vector3(0.0f, 0.0f, -1.0f));
	REQUIRE(EulerAngle(90.0f, 0.0f, 0.0f).getDirectionVector() == Vector3(-1.0f, 0.0f, 0.0f));
	REQUIRE(EulerAngle(-90.0f, 0.0f, 0.0f).getDirectionVector() == Vector3(1.0f, 0.0f, 0.0f));
	REQUIRE(EulerAngle(180.0f, 0.0f, 0.0f).getDirectionVector() == Vector3(0.0f, 0.0f, 1.0f));
	REQUIRE(EulerAngle(0.0f, 90.0f, 0.0f).getDirectionVector() == Vector3(0.0f, 1.0f, 0.0f));
	REQUIRE(EulerAngle(0.0f, -90.0f, 0.0f).getDirectionVector() == Vector3(0.0f, -1.0f, 0.0f));
	REQUIRE(EulerAngle(123.0f, 90.0f, 0.0f).getDirectionVector() == Vector3(0.0f, 1.0f, 0.0f));
}
