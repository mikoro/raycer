// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "catch/catch.hpp"

#include "Utils/ObjReader.h"
#include "Raytracing/Primitives/Triangle.h"

using namespace Raycer;

TEST_CASE("ObjReader functionality", "[objreader]")
{
	std::vector<Triangle> triangles = ObjReader::readFile("data/meshes/cube1.obj");
	REQUIRE(triangles.size() == 12);

	triangles = ObjReader::readFile("data/meshes/cube2.obj");
	REQUIRE(triangles.size() == 12);

	triangles = ObjReader::readFile("data/meshes/cube3.obj");
	REQUIRE(triangles.size() == 12);

	triangles = ObjReader::readFile("data/meshes/cube4.obj");
	REQUIRE(triangles.size() == 12);

	triangles = ObjReader::readFile("data/meshes/cube5.obj");
	REQUIRE(triangles.size() == 12);
}
