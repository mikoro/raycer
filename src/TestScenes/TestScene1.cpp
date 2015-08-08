// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <string>
#include <vector>

#include "TestScenes/TestScene.h"
#include "Raytracing/Scene.h"

using namespace Raycer;

Scene TestScene::createTestScene1()
{
	Scene scene;

	scene.tracer.maxIterations = 4;

	scene.multisampler.type = MultisampleType::NONE;
	scene.multisampler.multisamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 3.0, 6.0);
	scene.camera.orientation = EulerAngle(0.0, -20.0, 0.0);
	scene.camera.fov = 75.0;
	scene.camera.depthOfField = false;
	scene.camera.samples = 3;
	scene.camera.apertureSize = 0.1;
	scene.camera.focalLenght = 6.0;

	// FOG //

	scene.fog.enabled = false;
	scene.fog.color = Color(0.7, 0.7, 0.7);
	scene.fog.distance = 100.0;
	scene.fog.steepness = 4.0;
	scene.fog.heightDispersion = true;
	scene.fog.height = 20.0;
	scene.fog.heightSteepness = 0.5;

	// GROUND //

	ColorTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color = Color(1.0, 1.0, 1.0);
	groundTexture.intensity = 0.3;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.textureId = groundTexture.id;
	groundMaterial.ambientness = 1.0;
	groundMaterial.diffuseness = 1.0;
	groundMaterial.specularity = 0.2;
	groundMaterial.shininess = 2.0;
	groundMaterial.reflectance = 0.0;
	groundMaterial.transmittance = 0.0;
	groundMaterial.refractiveIndex = 1.0;

	Plane groundPlane;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();
	groundPlane.texcoordScale = Vector2(5.0, 5.0);

	scene.textures.colorTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// SPHERE 1 //

	ColorTexture sphere1Texture;
	sphere1Texture.id = 2;
	sphere1Texture.color = Color(1.0, 0.0, 0.0);
	sphere1Texture.intensity = 0.5;

	Material sphere1Material;
	sphere1Material.id = 2;
	sphere1Material.textureId = sphere1Texture.id;
	sphere1Material.ambientness = 1.0;
	sphere1Material.diffuseness = 1.0;
	sphere1Material.specularity = 1.0;
	sphere1Material.shininess = 32.0;
	sphere1Material.reflectance = 0.0;
	sphere1Material.transmittance = 0.0;
	sphere1Material.refractiveIndex = 1.0;

	Sphere sphere1;
	sphere1.materialId = sphere1Material.id;
	sphere1.texcoordScale = Vector2(1.0, 1.0);
	sphere1.position = Vector3(0.0, 1.0, 0.0);
	sphere1.radius = 1.0;

	scene.textures.colorTextures.push_back(sphere1Texture);
	scene.materials.push_back(sphere1Material);
	scene.primitives.spheres.push_back(sphere1);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;
	scene.lights.ambientLight.ambientOcclusion = false;
	scene.lights.ambientLight.samples = 3;
	scene.lights.ambientLight.distribution = 1.0;
	scene.lights.ambientLight.distance = 1.0;

	DirectionalLight directionalLight1;
	directionalLight1.color = Color(192, 191, 173);
	directionalLight1.direction = EulerAngle(45.0, -45.0, 0.0).getDirectionVector();
	directionalLight1.intensity = 0.8;

	scene.lights.directionalLights.push_back(directionalLight1);

	return scene;
}
