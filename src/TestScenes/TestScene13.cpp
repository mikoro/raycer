// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>

#include "Raytracing/Scene.h"

using namespace Raycer;

// bump mapping with spheres and squares
Scene Scene::createTestScene13()
{
	Scene scene;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 5.0, 5.0);
	scene.camera.orientation = EulerAngle(-25.0, 0.0, 0.0);

	// GROUND //

	ColorTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color = Color(1.0, 1.0, 1.0);
	groundTexture.intensity = 0.5;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.colorTextureId = groundTexture.id;

	Plane groundPlane;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();
	
	scene.textures.colorTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// SPHERE 1 //

	ColorTexture sphere1Texture1;
	sphere1Texture1.id = 2;
	sphere1Texture1.color = Color(1.0, 0.1, 0.1);
	sphere1Texture1.intensity = 0.5;

	PerlinNoiseTexture sphere1Texture2;
	sphere1Texture2.id = 3;
	sphere1Texture2.seed = 2345654;
	sphere1Texture2.intensity = 0.05;
	sphere1Texture2.scale = Vector3(10.0, 10.0, 10.0);
	sphere1Texture2.isFbm = true;

	Material sphere1Material;
	sphere1Material.id = 2;
	sphere1Material.colorTextureId = sphere1Texture1.id;
	sphere1Material.normalMapTextureId = sphere1Texture2.id;
	sphere1Material.diffuseReflectance = Color(0.5, 0.5, 0.5);
	sphere1Material.specularReflectance = Color(1.0, 1.0, 1.0);
	sphere1Material.shininess = 64.0;
	sphere1Material.nonShadowing = true;

	Sphere sphere1;
	sphere1.materialId = sphere1Material.id;
	sphere1.position = Vector3(-2.0, 1.0, 0.0);
	sphere1.radius = 1.0;

	scene.textures.colorTextures.push_back(sphere1Texture1);
	scene.textures.perlinNoiseTextures.push_back(sphere1Texture2);
	scene.materials.push_back(sphere1Material);
	scene.primitives.spheres.push_back(sphere1);

	// SPHERE 2 //

	ColorTexture sphere2Texture1;
	sphere2Texture1.id = 4;
	sphere2Texture1.color = Color(1.0, 0.1, 0.1);
	sphere2Texture1.intensity = 0.5;

	CellNoiseTexture sphere2Texture2;
	sphere2Texture2.id = 5;
	sphere2Texture2.seed = 2345654;
	sphere2Texture2.intensity = 0.1;
	sphere2Texture2.scale = Vector3(10.0, 10.0, 10.0);
	sphere2Texture2.combineType = CellNoiseCombineType::D2_MINUS_D1;

	Material sphere2Material;
	sphere2Material.id = 3;
	sphere2Material.colorTextureId = sphere2Texture1.id;
	sphere2Material.normalMapTextureId = sphere2Texture2.id;
	sphere2Material.diffuseReflectance = Color(0.5, 0.5, 0.5);
	sphere2Material.specularReflectance = Color(1.0, 1.0, 1.0);
	sphere2Material.shininess = 64.0;
	sphere2Material.nonShadowing = true;

	Sphere sphere2;
	sphere2.materialId = sphere2Material.id;
	sphere2.position = Vector3(2.0, 1.0, 0.0);
	sphere2.radius = 1.0;

	scene.textures.colorTextures.push_back(sphere2Texture1);
	scene.textures.cellNoiseTextures.push_back(sphere2Texture2);
	scene.materials.push_back(sphere2Material);
	scene.primitives.spheres.push_back(sphere2);

	// SQUARES //

	Mesh mesh1;
	mesh1.materialId = sphere1Material.id;
	mesh1.meshFilePath = "data/meshes/square.obj";
	mesh1.position = Vector3(-1.0, 0.0, -4.0);
	mesh1.scale = Vector3(2.0, 2.0, 2.0);

	scene.primitives.meshes.push_back(mesh1);

	Mesh mesh2;
	mesh2.materialId = sphere2Material.id;
	mesh2.meshFilePath = "data/meshes/square.obj";
	mesh2.position = Vector3(2.0, 0.0, -4.0);
	mesh2.scale = Vector3(2.0, 2.0, 2.0);

	scene.primitives.meshes.push_back(mesh2);

	ImageTexture mesh3Texture2;
	mesh3Texture2.id = 7;
	mesh3Texture2.imageFilePath = "data/images/bumpmap2.jpg";
	mesh3Texture2.applyGamma = false;
	mesh3Texture2.isBumpMap = true;
	mesh3Texture2.intensity = 5.0;

	Material mesh3Material;
	mesh3Material.id = 4;
	mesh3Material.colorTextureId = sphere2Texture1.id;
	mesh3Material.normalMapTextureId = mesh3Texture2.id;
	mesh3Material.diffuseReflectance = Color(0.5, 0.5, 0.5);
	//mesh3Material.specularReflectance = Color(1.0, 1.0, 1.0);
	mesh3Material.shininess = 4.0;
	mesh3Material.nonShadowing = true;

	Mesh mesh3;
	mesh3.materialId = mesh3Material.id;
	mesh3.meshFilePath = "data/meshes/square.obj";
	mesh3.position = Vector3(-4.0, 0.0, -4.0);
	mesh3.scale = Vector3(2.0, 2.0, 2.0);
	mesh3.orientation = EulerAngle(0.0, 0.0, 0.0);

	scene.textures.imageTextures.push_back(mesh3Texture2);
	scene.materials.push_back(mesh3Material);
	scene.primitives.meshes.push_back(mesh3);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.01;

	PointLight pointLight1;
	pointLight1.color = Color(1.0, 1.0, 1.0);
	pointLight1.intensity = 0.8;
	pointLight1.distance = 20.0;
	pointLight1.attenuation = 1.0;

	pointLight1.position = Vector3(5.0, 5.0, 5.0);
	scene.lights.pointLights.push_back(pointLight1);
	pointLight1.position = Vector3(-5.0, 5.0, 5.0);
	scene.lights.pointLights.push_back(pointLight1);

	return scene;
}
