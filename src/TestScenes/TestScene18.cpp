// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <random>

#include "Raytracing/Scene.h"
#include "Utils/ObjModelLoader.h"

using namespace Raycer;

// sunflower field
Scene Scene::createTestScene18()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;
	//scene.raytracer.cameraSamples = 5;

	scene.rootBVH.enabled = true;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 3.8, 0.0);
	scene.camera.orientation = EulerAngle(-8.0, 90.0, 0.0);
	scene.camera.focalDistance = 10.0;
	scene.camera.apertureSize = 0.02;

	// SPHERE 1 //

	ImageTexture sphere1Texture;
	sphere1Texture.id = 1;
	sphere1Texture.intensity = 1.2;
	sphere1Texture.imageFilePath = "data/images/sky.jpg";
	sphere1Texture.applyGamma = true;

	Material sphere1Material;
	sphere1Material.id = 1;
	sphere1Material.ambientMapTextureId = sphere1Texture.id;
	sphere1Material.diffuseMapTextureId = sphere1Texture.id;
	sphere1Material.texcoordScale = Vector2(1.0, 1.0);
	sphere1Material.skipLighting = true;
	sphere1Material.nonShadowing = true;

	Sphere sphere1;
	sphere1.materialId = sphere1Material.id;
	sphere1.position = Vector3(0.0, 0.0, 0.0);
	sphere1.radius = 1000.0;
	sphere1.uvMapType = SphereUVMapType::SPHERICAL;

	scene.textures.imageTextures.push_back(sphere1Texture);
	scene.materials.push_back(sphere1Material);
	scene.primitives.spheres.push_back(sphere1);

	// GROUND //

	ImageTexture groundTexture;
	groundTexture.id = 2;
	groundTexture.intensity = 0.5;
	groundTexture.imageFilePath = "data/images/dirt.jpg";
	groundTexture.applyGamma = true;

	Material groundMaterial;
	groundMaterial.id = 2;
	groundMaterial.ambientMapTextureId = groundTexture.id;
	groundMaterial.diffuseMapTextureId = groundTexture.id;
	groundMaterial.texcoordScale = Vector2(5.0, 5.0);
	groundMaterial.nonShadowing = true;

	Plane groundPlane;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();

	scene.textures.imageTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// MESHES //

	ModelLoaderInfo modelInfo;
	modelInfo.modelFilePath = "data/meshes/sunflower/sunflower.obj";
	modelInfo.addAllInstance = true;
	modelInfo.allGroupId = 1;
	modelInfo.scale = Vector3(0.1, 0.1, 0.1);
	modelInfo.rotate = EulerAngle(0.0, 90.0, 0.0);
	modelInfo.translate = Vector3(0.0, 1.5, 0.0);
	
	scene.models.push_back(modelInfo);

	std::mt19937 gen(230947887);
	std::uniform_real_distribution<double> rotationDist(-10.0, 10.0);
	std::uniform_real_distribution<double> translateDist(0.0, 0.5);

	for (int y = -200; y < 200; y += 1)
	{
		for (int x = -400; x < 0; x += 1)
		{
			Instance instance;
			instance.primitiveId = 1;
			instance.translate = Vector3(x + translateDist(gen), translateDist(gen), y + translateDist(gen));
			instance.rotate = EulerAngle(0.0, rotationDist(gen), 30.0 + rotationDist(gen));

			scene.primitives.instances.push_back(instance);
		}
	}

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	DirectionalLight directionalLight1;
	directionalLight1.color = Color(1.0, 1.0, 1.0);

	directionalLight1.intensity = 0.6;
	directionalLight1.direction = EulerAngle(-40.0, 35.0, 0.0).getDirection();
	scene.lights.directionalLights.push_back(directionalLight1);

	return scene;
}
