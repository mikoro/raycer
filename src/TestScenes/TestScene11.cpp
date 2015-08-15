// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// atrium scene
Scene Scene::createTestScene11()
{
	Scene scene;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 25.0, 0.0);
	scene.camera.orientation = EulerAngle(90.0, 0.0, 0.0);
	
	// MESH 1 //

	ColorTexture mesh1Texture;
	mesh1Texture.id = 1;
	mesh1Texture.color = Color(1.0, 1.0, 1.0);
	mesh1Texture.intensity = 1.0;

	Material mesh1Material;
	mesh1Material.id = 1;
	mesh1Material.textureId = mesh1Texture.id;

	Mesh mesh1;
	mesh1.materialId = mesh1Material.id;
	mesh1.meshFilePath = "data/meshes/sponza2.obj";
	mesh1.position = Vector3(0.0, 0.0, 0.0);
	mesh1.scale = Vector3(0.05, 0.05, 0.05);
	mesh1.orientation = EulerAngle(0.0, 0.0, 0.0);

	scene.textures.colorTextures.push_back(mesh1Texture);
	scene.materials.push_back(mesh1Material);
	scene.primitives.meshes.push_back(mesh1);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.01;

	PointLight pointLight1;
	pointLight1.color = Color(1.0, 1.0, 1.0);
	pointLight1.intensity = 1.0;
	pointLight1.position = Vector3(28.0, 38.0, 0.0);
	pointLight1.distance = 50.0;
	pointLight1.attenuation = 1.0;

	PointLight pointLight2;
	pointLight2.color = Color(1.0, 1.0, 1.0);
	pointLight2.intensity = 1.0;
	pointLight2.position = Vector3(-28.0, 38.0, 0.0);
	pointLight2.distance = 50.0;
	pointLight2.attenuation = 1.0;

	scene.lights.pointLights.push_back(pointLight1);
	scene.lights.pointLights.push_back(pointLight2);

	return scene;
}
