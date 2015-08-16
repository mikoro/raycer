// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <random>
#include <stdexcept>

#include "Raytracing/Scene.h"

using namespace Raycer;

// instancing
Scene Scene::createTestScene10()
{
	Scene scene;

	scene.rootBVH.enabled = true;

	scene.misc.backgroundColor = Color::WHITE;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 5.0, 7.0);
	scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);

	// MESH 1 //

	ColorTexture mesh1Texture;
	mesh1Texture.id = 1;
	mesh1Texture.color = Color(1.0, 1.0, 1.0);
	mesh1Texture.intensity = 0.8;

	Material mesh1Material;
	mesh1Material.id = 1;
	mesh1Material.textureId = mesh1Texture.id;

	Mesh mesh1;
	mesh1.materialId = mesh1Material.id;
	mesh1.meshFilePath = "data/meshes/monkey3.obj";
	mesh1.position = Vector3(0.0, 0.0, 0.0);
	mesh1.scale = Vector3(6.0, 6.0, 6.0);
	mesh1.orientation = EulerAngle(0.0, 0.0, 0.0);

	scene.textures.colorTextures.push_back(mesh1Texture);
	scene.materials.push_back(mesh1Material);
	scene.primitives.meshes.push_back(mesh1);

	// INSTANCES

	std::mt19937 gen(210982);
	std::uniform_real_distribution<double> scaleDist(1.0, 2.0);
	std::uniform_real_distribution<double> rotationDist(0.0, 180.0);

	int count = 2;

	/*
	for (int z = -100; z < 100; z += 20)
	{
		for (int y = -100; y < 100; y += 20)
		{
			for (int x = -100; x < 100; x += 20)
			{
				ColorTexture instance1Texture;
				instance1Texture.id = count;
				instance1Texture.color = Color::random(gen);
				instance1Texture.intensity = 0.8;

				Material instance1Material;
				instance1Material.id = count;
				instance1Material.textureId = instance1Texture.id;

				double scale = scaleDist(gen);

				Instance instance1;
				instance1.materialId = instance1Material.id;
				instance1.primitive = &scene.primitives.meshes.back();
				instance1.rotate = EulerAngle(rotationDist(gen), rotationDist(gen), rotationDist(gen));
				instance1.scale = Vector3(scale, scale, scale);
				instance1.translate = Vector3(x, y, z);

				scene.textures.colorTextures.push_back(instance1Texture);
				scene.materials.push_back(instance1Material);
				scene.primitives.instances.push_back(instance1);
				count++;
			}
		}
	}
	*/

	for (int y = -100; y < 100; y += 20)
	{
		for (int x = -100; x < 100; x += 20)
		{
			ColorTexture instance1Texture;
			instance1Texture.id = count;
			instance1Texture.color = Color::random(gen);
			instance1Texture.intensity = 0.8;

			Material instance1Material;
			instance1Material.id = count;
			instance1Material.textureId = instance1Texture.id;

			//double scale = scaleDist(gen);

			Instance instance1;
			instance1.materialId = instance1Material.id;
			instance1.primitive = &scene.primitives.meshes.back();
			instance1.rotate = EulerAngle(rotationDist(gen), rotationDist(gen), rotationDist(gen));
			//instance1.scale = Vector3(scale, scale, scale);
			instance1.translate = Vector3(x, 0.0, y);

			scene.textures.colorTextures.push_back(instance1Texture);
			scene.materials.push_back(instance1Material);
			scene.primitives.instances.push_back(instance1);
			count++;
		}
	}

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	DirectionalLight directionalLight1;
	directionalLight1.color = Color(1.0, 1.0, 1.0);
	directionalLight1.intensity = 1.0;
	directionalLight1.direction = EulerAngle(30.0, -10.0, 0.0).getDirectionVector();

	scene.lights.directionalLights.push_back(directionalLight1);

	return scene;
}
