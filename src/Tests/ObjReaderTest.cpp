// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <vector>

#include "catch/catch.hpp"

#include "Utils/ObjReader.h"
#include "Raytracing/Primitives/Triangle.h"

using namespace Raycer;

TEST_CASE("ObjReader functionality", "[objreader]")
{
	std::vector<Triangle> triangles = ObjReader::getTriangles("data/meshes/cube1.obj");
	REQUIRE(triangles.size() == 12);

	triangles = ObjReader::getTriangles("data/meshes/cube2.obj");
	REQUIRE(triangles.size() == 12);

	triangles = ObjReader::getTriangles("data/meshes/cube3.obj");
	REQUIRE(triangles.size() == 12);

	triangles = ObjReader::getTriangles("data/meshes/cube4.obj");
	REQUIRE(triangles.size() == 12);

	triangles = ObjReader::getTriangles("data/meshes/cube5.obj");
	REQUIRE(triangles.size() == 12);

	ObjReaderResult result = ObjReader::getMeshes("data/meshes/cornellbox.obj", Vector3(1.0, 1.0, 1.0), 0);
	REQUIRE(result.materials.size() == 8);
	REQUIRE(result.meshes.size() == 8);
}
