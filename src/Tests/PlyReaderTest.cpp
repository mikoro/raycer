// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "catch/catch.hpp"

#include "Utils/PlyReader.h"
#include "Raytracing/Primitives/Triangle.h"

using namespace Raycer;

TEST_CASE("PlyReader functionality", "[plyreader]")
{
	std::vector<Triangle> triangles = PlyReader::readFile("data/meshes/cube1.ply");
	REQUIRE(triangles.size() == 12);

	triangles = PlyReader::readFile("data/meshes/cube2.ply");
	REQUIRE(triangles.size() == 12);

	triangles = PlyReader::readFile("data/meshes/cube3.ply");
	REQUIRE(triangles.size() == 12);
}
