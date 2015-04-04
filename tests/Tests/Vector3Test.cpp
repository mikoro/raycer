// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "catch/catch.hpp"

#include "Math/Vector3.h"
#include "Math/MathUtils.h"

using namespace Raycer;

TEST_CASE("Vector3 functionality", "[vector3]")
{
	Vector3 v1(1.0f, 2.0f, 3.0f);
	Vector3 v2(3.0f, 2.0f, 1.0f);
	Vector3 v3;

	REQUIRE(v1.x == 1.0f);
	REQUIRE(v1.y == 2.0f);
	REQUIRE(v1.z == 3.0f);

	REQUIRE(v3.x == 0.0f);
	REQUIRE(v3.y == 0.0f);
	REQUIRE(v3.z == 0.0f);

	REQUIRE(v1 == Vector3(1.0f, 2.0f, 3.0f));
	REQUIRE(v1 != Vector3(2.0f, 2.0f, 3.0f));

	REQUIRE((v1 + v2) == Vector3(4.0f, 4.0f, 4.0f));
	REQUIRE((v1 - v2) == Vector3(-2.0f, 0.0f, 2.0f));
	REQUIRE((v1 * 2.0f) == Vector3(2.0f, 4.0f, 6.0f));
	REQUIRE((2.0f * v1) == Vector3(2.0f, 4.0f, 6.0f));
	REQUIRE((v1 / 2.0f) == Vector3(0.5f, 1.0f, 1.5f));
	REQUIRE((-v1) == Vector3(-1.0f, -2.0f, -3.0f));
	REQUIRE((v3 += v1) == Vector3(1.0f, 2.0f, 3.0f));
	REQUIRE((v3 -= v2) == Vector3(-2.0f, 0.0f, 2.0f));
	REQUIRE((v3 *= 2.0f) == Vector3(-4.0f, 0.0f, 4.0f));
	REQUIRE((v3 /= 4.0f) == Vector3(-1.0f, 0.0f, 1.0f));

	Vector3 v4(0.0f, 0.0f, 0.0f);
	Vector3 v5(1.0f, 2.0f, 3.0f);
	Vector3 v6(1.0f, 2.0f, 3.0f);
	
	v5.normalize();
	
	REQUIRE(MathUtils::almostSame(v1.length(), 3.741657387f));
	REQUIRE(MathUtils::almostSame(v1.lengthSquared(), 14.0f));
	REQUIRE(v5 == Vector3(0.2672612419f, 0.5345224838f, 0.8017837257f));
	REQUIRE(v5 == v6.normalized());
	REQUIRE(v4.isZero());
	REQUIRE(!v5.isZero());
	REQUIRE(v5.isNormal());
	REQUIRE(!v6.isNormal());
	REQUIRE(MathUtils::almostSame(v1.dot(v2), 10.0f));
	REQUIRE(v1.cross(v2) == Vector3(-4.0f, 8.0f, -4.0f));
	REQUIRE(Vector3::RIGHT.cross(Vector3::UP) == Vector3::FORWARD);
	REQUIRE(Vector3(2.0f, -2.0f).reflect(Vector3(0.0f, 1.0f, 0.0f)) == Vector3(2.0f, 2.0f, 0.0f));
	REQUIRE(Vector3(2.0f, -2.0f).reflect(Vector3(0.0f, -1.0f, 0.0f)) == Vector3(2.0f, 2.0f, 0.0f));
	REQUIRE(Vector3::lerp(v4, v6, 0.5f) == Vector3(0.5f, 1.0f, 1.5f));
}
