// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// monkey mesh on a plane with two colored point lights
Scene Scene::createTestScene3()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 5.0, 8.0);
	scene.camera.orientation = EulerAngle(-26.0, 0.0, 0.0);

	// GROUND //

	ColorTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color = Color(1.0, 1.0, 1.0);
	groundTexture.intensity = 0.3;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.ambientMapTextureId = groundTexture.id;
	groundMaterial.diffuseMapTextureId = groundTexture.id;

	Plane groundPlane;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();

	scene.textures.colorTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// MESH 1 //

	ColorTexture mesh1Texture;
	mesh1Texture.id = 2;
	mesh1Texture.color = Color(1.0, 1.0, 1.0);
	mesh1Texture.intensity = 0.8;

	Material mesh1Material;
	mesh1Material.id = 2;
	mesh1Material.ambientMapTextureId = mesh1Texture.id;
	mesh1Material.diffuseMapTextureId = mesh1Texture.id;

	ModelLoaderInfo modelInfo1;
	modelInfo1.modelFilePath = "data/meshes/monkey3.obj";
	modelInfo1.defaultMaterialId = mesh1Material.id;
	modelInfo1.addAllInstance = true;
	modelInfo1.allGroupId = 1;
	modelInfo1.scale = Vector3(6.0, 6.0, 6.0);
	modelInfo1.rotate = EulerAngle(0.0, 0.0, 0.0);
	modelInfo1.translate = Vector3(0.0, 0.0, 0.0);

	scene.textures.colorTextures.push_back(mesh1Texture);
	scene.materials.push_back(mesh1Material);
	scene.models.push_back(modelInfo1);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	PointLight pointLight1;
	pointLight1.color = Color(1.0, 0.25, 0.25);
	pointLight1.intensity = 1.5;
	pointLight1.position = Vector3(5.0, 5.0, 5.0);
	pointLight1.distance = 20.0;
	pointLight1.attenuation = 1.0;

	PointLight pointLight2;
	pointLight2.color = Color(0.25, 0.25, 1.0);
	pointLight2.intensity = 1.5;
	pointLight2.position = Vector3(-5.0, 5.0, 5.0);
	pointLight2.distance = 20.0;
	pointLight2.attenuation = 1.0;

	scene.lights.pointLights.push_back(pointLight1);
	scene.lights.pointLights.push_back(pointLight2);

	return scene;
}
