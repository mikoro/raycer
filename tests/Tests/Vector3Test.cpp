// Copyright � 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "catch/catch.hpp"

#include "Math/Vector3.h"
#include "Math/MathUtils.h"

using namespace Raycer;

TEST_CASE("Vector3 functionality", "[vector3]")
{
	Vector3 v1(1.0, 2.0, 3.0);
	Vector3 v2(3.0, 2.0, 1.0);
	Vector3 v3;

	REQUIRE(v1.x == 1.0);
	REQUIRE(v1.y == 2.0);
	REQUIRE(v1.z == 3.0);

	REQUIRE(v3.x == 0.0);
	REQUIRE(v3.y == 0.0);
	REQUIRE(v3.z == 0.0);

	REQUIRE(v1 == Vector3(1.0, 2.0, 3.0));
	REQUIRE(v1 != Vector3(2.0, 2.0, 3.0));

	REQUIRE((v1 + v2) == Vector3(4.0, 4.0, 4.0));
	REQUIRE((v1 - v2) == Vector3(-2.0, 0.0, 2.0));
	REQUIRE((v1 * 2.0) == Vector3(2.0, 4.0, 6.0));
	REQUIRE((2.0 * v1) == Vector3(2.0, 4.0, 6.0));
	REQUIRE((v1 / 2.0) == Vector3(0.5, 1.0, 1.5));
	REQUIRE((-v1) == Vector3(-1.0, -2.0, -3.0));
	REQUIRE((v3 += v1) == Vector3(1.0, 2.0, 3.0));
	REQUIRE((v3 -= v2) == Vector3(-2.0, 0.0, 2.0));
	REQUIRE((v3 *= 2.0) == Vector3(-4.0, 0.0, 4.0));
	REQUIRE((v3 /= 4.0) == Vector3(-1.0, 0.0, 1.0));

	Vector3 v4(0.0, 0.0, 0.0);
	Vector3 v5(1.0, 2.0, 3.0);
	Vector3 v6(1.0, 2.0, 3.0);

	v5.normalize();

	REQUIRE(MathUtils::almostSame(v1.length(), 3.7416573867739413));
	REQUIRE(MathUtils::almostSame(v1.lengthSquared(), 14.0));
	REQUIRE(v5 == Vector3(0.26726124191242440, 0.53452248382484879, 0.80178372573727319));
	REQUIRE(v5 == v6.normalized());
	REQUIRE(v4.isZero());
	REQUIRE(!v5.isZero());
	REQUIRE(v5.isNormal());
	REQUIRE(!v6.isNormal());
	REQUIRE(MathUtils::almostSame(v1.dot(v2), 10.0));
	REQUIRE(v1.cross(v2) == Vector3(-4.0, 8.0, -4.0));
	REQUIRE(Vector3::RIGHT.cross(Vector3::UP) == Vector3::FORWARD);
	REQUIRE(Vector3(2.0, -2.0).reflect(Vector3(0.0, 1.0, 0.0)) == Vector3(2.0, 2.0, 0.0));
	REQUIRE(Vector3(2.0, -2.0).reflect(Vector3(0.0, -1.0, 0.0)) == Vector3(2.0, 2.0, 0.0));
	REQUIRE(Vector3::lerp(v4, v6, 0.5) == Vector3(0.5, 1.0, 1.5));
}
