// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <random>

#include "Raytracing/Scene.h"
#include "Utils/ObjModelLoader.h"

using namespace Raycer;

// sunflower field
Scene Scene::createTestScene17()
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

	// SKY SPHERE //

	ImageTexture sphereTexture;
	sphereTexture.id = 1;
	sphereTexture.intensity = 1.2;
	sphereTexture.imageFilePath = "data/images/sky.jpg";

	Material sphereMaterial;
	sphereMaterial.id = 1;
	sphereMaterial.ambientMapTextureId = sphereTexture.id;
	sphereMaterial.diffuseMapTextureId = sphereTexture.id;
	sphereMaterial.texcoordScale = Vector2(1.0, 1.0);
	sphereMaterial.skipLighting = true;
	sphereMaterial.nonShadowing = true;

	Sphere sphere;
	sphere.id = 1;
	sphere.materialId = sphereMaterial.id;
	sphere.radius = 1000.0;
	sphere.uvMapType = SphereUVMapType::SPHERICAL;

	scene.textures.imageTextures.push_back(sphereTexture);
	scene.materials.push_back(sphereMaterial);
	scene.primitives.spheres.push_back(sphere);

	// GROUND //

	ImageTexture groundTexture;
	groundTexture.id = 2;
	groundTexture.intensity = 0.5;
	groundTexture.imageFilePath = "data/images/dirt.jpg";

	Material groundMaterial;
	groundMaterial.id = 2;
	groundMaterial.ambientMapTextureId = groundTexture.id;
	groundMaterial.diffuseMapTextureId = groundTexture.id;
	groundMaterial.texcoordScale = Vector2(5.0, 5.0);
	groundMaterial.nonShadowing = true;

	Plane groundPlane;
	groundPlane.id = 2;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();

	scene.textures.imageTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// SUNFLOWER MODEL //

	ModelLoaderInfo modelInfo(ModelLoaderInfoType::COMBINED_GROUP);
	modelInfo.modelFilePath = "data/meshes/sunflower/sunflower.obj";
	modelInfo.combinedGroupId = 3;
	modelInfo.enableCombinedGroupInstance = false;
	modelInfo.idStartOffset = 4;
	modelInfo.scale = Vector3(0.1, 0.1, 0.1);
	modelInfo.rotate = EulerAngle(0.0, 90.0, 0.0);
	modelInfo.translate = Vector3(0.0, 1.5, 0.0);

	scene.models.push_back(modelInfo);

	// INSTANCES //

	std::mt19937 gen(230947887);
	std::uniform_real_distribution<double> rotationDist(-10.0, 10.0);
	std::uniform_real_distribution<double> translateDist(0.0, 0.5);

	int currentId = 1000000;
	double xstep = 1.0;

	for (double x = 0.0; x > -400.0; x -= xstep)
	{
		for (double z = -200.0; z < 200.0; z += 1.0)
		{
			Instance instance;
			instance.id = currentId;
			instance.primitiveId = modelInfo.combinedGroupId;
			instance.translate = Vector3(x + translateDist(gen), translateDist(gen), z + translateDist(gen));
			instance.rotate = EulerAngle(0.0, rotationDist(gen), 30.0 + rotationDist(gen));

			scene.primitives.instances.push_back(instance);

			++currentId;
		}

		if (x < -20.0)
			xstep += 1.0;
	}

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	DirectionalLight directionalLight;
	directionalLight.color = Color(1.0, 1.0, 1.0);

	directionalLight.intensity = 0.6;
	directionalLight.direction = EulerAngle(-40.0, 35.0, 0.0).getDirection();
	scene.lights.directionalLights.push_back(directionalLight);

	return scene;
}
