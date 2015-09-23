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

	// MESH 1 //

	ColorTexture mesh1Texture;
	mesh1Texture.id = 1;
	mesh1Texture.color = Color(1.0, 1.0, 1.0);
	mesh1Texture.intensity = 0.8;

	Material mesh1Material;
	mesh1Material.id = 1;
	mesh1Material.ambientMapTextureId = mesh1Texture.id;
	mesh1Material.diffuseMapTextureId = mesh1Texture.id;

	ModelLoaderInfo modelInfo1;
	modelInfo1.modelFilePath = "data/meshes/monkey3.obj";
	modelInfo1.defaultMaterialId = mesh1Material.id;
	modelInfo1.addAllGroup = true;
	modelInfo1.allGroupId = 1;
	modelInfo1.scale = Vector3(6.0, 6.0, 6.0);
	modelInfo1.rotate = EulerAngle(0.0, 0.0, 0.0);
	modelInfo1.translate = Vector3(0.0, 0.0, 0.0);

	scene.textures.colorTextures.push_back(mesh1Texture);
	scene.materials.push_back(mesh1Material);
	scene.models.push_back(modelInfo1);

	// INSTANCES

	std::mt19937 gen(1230927546);
	std::uniform_real_distribution<double> scaleDist(0.5, 2.0);
	std::uniform_real_distribution<double> rotationDist(-45.0, 45.0);
	std::uniform_real_distribution<double> translateDist(-8.0, 8.0);

	int count = 2;

	for (int z = -200; z < 200; z += 20)
	{
		for (int y = -200; y < 200; y += 20)
		{
			for (int x = -200; x < 200; x += 20)
			{
				ColorTexture instance1Texture;
				instance1Texture.id = count;
				instance1Texture.color = Color::random(gen);
				instance1Texture.intensity = 0.8;

				Material instance1Material;
				instance1Material.id = count;
				instance1Material.ambientMapTextureId = instance1Texture.id;
				instance1Material.diffuseMapTextureId = instance1Texture.id;

				double scale = scaleDist(gen);

				Instance instance1;
				instance1.materialId = instance1Material.id;
				instance1.primitiveId = 1;
				instance1.changePrimitive = true;
				instance1.scale = Vector3(scale, scale, scale);
				instance1.rotate = EulerAngle(0.0, 0.0, rotationDist(gen));
				instance1.translate = Vector3(x + translateDist(gen), y + translateDist(gen), z + translateDist(gen));

				scene.textures.colorTextures.push_back(instance1Texture);
				scene.materials.push_back(instance1Material);
				scene.primitives.instances.push_back(instance1);
				count++;
			}
		}
	}

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	DirectionalLight directionalLight1;
	directionalLight1.color = Color(1.0, 1.0, 1.0);
	directionalLight1.intensity = 0.8;

	directionalLight1.direction = EulerAngle(0.0, 45.0, 0.0).getDirection();
	scene.lights.directionalLights.push_back(directionalLight1);

	directionalLight1.direction = EulerAngle(0.0, -45.0, 0.0).getDirection();
	scene.lights.directionalLights.push_back(directionalLight1);

	return scene;
}
