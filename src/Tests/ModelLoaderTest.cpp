// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#ifdef RUN_UNIT_TESTS

#include "catch/catch.hpp"

#include "Utils/ModelLoader.h"

using namespace Raycer;

TEST_CASE("ModelLoader functionality", "[modelloader]")
{
	ModelLoaderInfo info;
	info.modelFilePath = "data/meshes/cube1.obj";
	ModelLoaderResult result = ModelLoader::readObjFile(info);
	REQUIRE(result.triangles.size() == 12);

	info.modelFilePath = "data/meshes/cube2.obj";
	result = ModelLoader::readObjFile(info);
	REQUIRE(result.triangles.size() == 12);

	info.modelFilePath = "data/meshes/cube3.obj";
	result = ModelLoader::readObjFile(info);
	REQUIRE(result.triangles.size() == 12);

	info.modelFilePath = "data/meshes/cube4.obj";
	result = ModelLoader::readObjFile(info);
	REQUIRE(result.triangles.size() == 12);

	info.modelFilePath = "data/meshes/cube5.obj";
	result = ModelLoader::readObjFile(info);
	REQUIRE(result.triangles.size() == 12);

	info.modelFilePath = "data/meshes/cube1.ply";
	result = ModelLoader::readPlyFile(info);
	REQUIRE(result.triangles.size() == 12);

	info.modelFilePath = "data/meshes/cube2.ply";
	result = ModelLoader::readPlyFile(info);
	REQUIRE(result.triangles.size() == 12);

	info.modelFilePath = "data/meshes/cube3.ply";
	result = ModelLoader::readPlyFile(info);
	REQUIRE(result.triangles.size() == 12);
}

#endif
