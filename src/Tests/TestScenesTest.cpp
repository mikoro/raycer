// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#ifdef RUN_UNIT_TESTS

#include "catch/catch.hpp"

#include "Raytracing/Scene.h"

using namespace Raycer;

TEST_CASE("TestScenes functionality", "[testscenes]")
{
	std::vector<Scene> scenes;

	scenes.push_back(Scene::createTestScene1());
	scenes.push_back(Scene::createTestScene2());
	scenes.push_back(Scene::createTestScene3());
	scenes.push_back(Scene::createTestScene4());
	scenes.push_back(Scene::createTestScene5());
	scenes.push_back(Scene::createTestScene6());
	scenes.push_back(Scene::createTestScene7());
	scenes.push_back(Scene::createTestScene8());
	scenes.push_back(Scene::createTestScene9());
	scenes.push_back(Scene::createTestScene10());
	scenes.push_back(Scene::createTestScene11());
	scenes.push_back(Scene::createTestScene12());
	scenes.push_back(Scene::createTestScene13());
	scenes.push_back(Scene::createTestScene14());
	scenes.push_back(Scene::createTestScene15());
	scenes.push_back(Scene::createTestScene16());
	scenes.push_back(Scene::createTestScene17());
	scenes.push_back(Scene::createTestScene18());
	scenes.push_back(Scene::createTestScene19());

	int sceneCount = 0;

	for (const Scene& scene : scenes)
	{
		sceneCount++;

		// skip large scenes
		if (sceneCount == 5 || sceneCount == 9 || sceneCount == 17)
			continue;

		scene.saveToFile(tfm::format("scene%d.json", sceneCount));
		scene.saveToFile(tfm::format("scene%d.xml", sceneCount));
		scene.saveToFile(tfm::format("scene%d.bin", sceneCount));
	}
}

#endif
