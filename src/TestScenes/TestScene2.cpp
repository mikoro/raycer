// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "TestScenes/TestScene.h"
#include "Raytracing/Scene.h"

using namespace Raycer;

// monkey mesh on a plane
Scene TestScene::createTestScene2()
{
	Scene scene;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 5.0, 7.0);
	scene.camera.orientation = EulerAngle(0.0, -30.0, 0.0);

	// GROUND //

	ColorTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color = Color(1.0, 1.0, 1.0);
	groundTexture.intensity = 0.3;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.textureId = groundTexture.id;

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
	mesh1Material.textureId = mesh1Texture.id;

	Mesh mesh1;
	mesh1.materialId = mesh1Material.id;
	mesh1.meshFilePath = "data/meshes/monkey2.obj";
	mesh1.position = Vector3(0.0, 2.0, 0.0);
	mesh1.scale = Vector3(2.0, 2.0, 2.0);

	scene.textures.colorTextures.push_back(mesh1Texture);
	scene.materials.push_back(mesh1Material);
	scene.primitives.meshes.push_back(mesh1);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	PointLight pointLight1;
	pointLight1.color = Color(1.0, 1.0, 1.0);
	pointLight1.intensity = 1.5;
	pointLight1.position = Vector3(5.0, 5.0, 5.0);
	pointLight1.distance = 20.0;
	pointLight1.attenuation = 1.0;

	scene.lights.pointLights.push_back(pointLight1);

	return scene;
}
