// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <vector>

#include "catch/catch.hpp"
#include "tinyformat/tinyformat.h"

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
	//scenes.push_back(Scene::createTestScene7());
	scenes.push_back(Scene::createTestScene8());
	scenes.push_back(Scene::createTestScene9());
	scenes.push_back(Scene::createTestScene10());
	scenes.push_back(Scene::createTestScene11());

	int sceneCount = 0;

	for (const Scene& scene : scenes)
	{
		sceneCount++;

		scene.saveToFile(tfm::format("scene%d.json", sceneCount));
		scene.saveToFile(tfm::format("scene%d.xml", sceneCount));
		scene.saveToFile(tfm::format("scene%d.bin", sceneCount));
	}
}
