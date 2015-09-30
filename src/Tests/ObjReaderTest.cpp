// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#ifdef RUN_UNIT_TESTS

#include "Utils/ObjReader.h"
#include "Raytracing/Primitives/Triangle.h"
#include "Math/Vector3.h"

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

	ObjReaderResult result = ObjReader::getMeshes("data/meshes/cornellbox.obj", Vector3(1.0, 1.0, 1.0), EulerAngle(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 0.0), false, 0);
	REQUIRE(result.materials.size() == 9);
	REQUIRE(result.meshes.size() == 8);
}

#endif
