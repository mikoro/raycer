// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "catch/catch.hpp"

#include "Utils/ObjReader.h"
#include "Raytracing/Primitives/Triangle.h"

using namespace Raycer;

TEST_CASE("ObjReader functionality", "[objreader]")
{
	ObjReader::readFile("data/meshes/cube.obj");
}
