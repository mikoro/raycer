// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <vector>

#include "catch/catch.hpp"
#include "tinyformat/tinyformat.h"

#include "TestScenes/TestScene.h"
#include "Raytracing/Scene.h"

using namespace Raycer;

TEST_CASE("TestScenes functionality", "[testscenes]")
{
	std::vector<Scene> scenes;

	scenes.push_back(TestScene::createTestScene1());
	scenes.push_back(TestScene::createTestScene2());

	int sceneCount = 0;

	for (const Scene& scene : scenes)
	{
		sceneCount++;

		scene.saveToFile(tfm::format("scene%d.json", sceneCount));
		scene.saveToFile(tfm::format("scene%d.xml", sceneCount));
		scene.saveToFile(tfm::format("scene%d.bin", sceneCount));
	}
}
