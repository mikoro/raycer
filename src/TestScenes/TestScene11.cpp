// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <random>

#include "Raytracing/Scene.h"

using namespace Raycer;

// instancing with a lot of monkeys
Scene Scene::createTestScene11()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	scene.rootBVH.enabled = true;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 0.0, 0.0);
	scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);

	// MONKEY MESH //

	ModelLoaderInfo monkeyModelInfo;
	monkeyModelInfo.modelFilePath = "data/meshes/monkey3.obj";
	monkeyModelInfo.combinedGroupId = 1;
	monkeyModelInfo.enableCombinedGroupInstance = false;
	monkeyModelInfo.scale = Vector3(6.0, 6.0, 6.0);

	scene.models.push_back(monkeyModelInfo);

	// INSTANCES

	std::mt19937 gen(1230927546);
	std::uniform_real_distribution<double> scaleDist(0.5, 2.0);
	std::uniform_real_distribution<double> rotationDist(-45.0, 45.0);
	std::uniform_real_distribution<double> translateDist(-8.0, 8.0);

	int currentId = 2;

	for (int z = -200; z < 200; z += 20)
	{
		for (int y = -200; y < 200; y += 20)
		{
			for (int x = -200; x < 200; x += 20)
			{
				Material instanceMaterial;
				instanceMaterial.id = currentId;
				instanceMaterial.ambientReflectance = Color::random(gen) * 0.8;
				instanceMaterial.diffuseReflectance = instanceMaterial.ambientReflectance;

				double scale = scaleDist(gen);

				Instance instance;
				instance.materialId = instanceMaterial.id;
				instance.primitiveId = monkeyModelInfo.combinedGroupId;
				instance.changePrimitive = true;
				instance.scale = Vector3(scale, scale, scale);
				instance.rotate = EulerAngle(0.0, 0.0, rotationDist(gen));
				instance.translate = Vector3(x + translateDist(gen), y + translateDist(gen), z + translateDist(gen));

				scene.materials.push_back(instanceMaterial);
				scene.primitives.instances.push_back(instance);

				++currentId;
			}
		}
	}

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	DirectionalLight directionalLight;
	directionalLight.color = Color(1.0, 1.0, 1.0);
	directionalLight.intensity = 0.8;

	directionalLight.direction = EulerAngle(0.0, 45.0, 0.0).getDirection();
	scene.lights.directionalLights.push_back(directionalLight);

	directionalLight.direction = EulerAngle(0.0, -45.0, 0.0).getDirection();
	scene.lights.directionalLights.push_back(directionalLight);

	return scene;
}
