// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// bump/normal mapping
Scene Scene::createTestScene13()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(-0.18, 1.65, 3.59);
	scene.camera.orientation = EulerAngle(-9.22, 17.56, 0.0);

	// SPHERE 1 //

	ColorTexture sphere1Texture1;
	sphere1Texture1.id = 1;
	sphere1Texture1.color = Color(1.0, 0.1, 0.1);
	sphere1Texture1.intensity = 0.5;

	PerlinNoiseTexture sphere1Texture2;
	sphere1Texture2.id = 2;
	sphere1Texture2.seed = 2345654;
	sphere1Texture2.intensity = 0.05;
	sphere1Texture2.scale = Vector3(10.0, 10.0, 10.0);
	sphere1Texture2.isFbm = true;

	Material sphere1Material;
	sphere1Material.id = 1;
	sphere1Material.colorTextureId = sphere1Texture1.id;
	sphere1Material.normalMapTextureId = sphere1Texture2.id;
	sphere1Material.diffuseReflectance = Color(0.5, 0.5, 0.5);
	sphere1Material.specularReflectance = Color(1.0, 1.0, 1.0);
	sphere1Material.shininess = 64.0;
	sphere1Material.nonShadowing = true;

	Sphere sphere1;
	sphere1.materialId = sphere1Material.id;
	sphere1.position = Vector3(0.0, 1.0, 0.0);
	sphere1.radius = 1.0;

	scene.textures.colorTextures.push_back(sphere1Texture1);
	scene.textures.perlinNoiseTextures.push_back(sphere1Texture2);
	scene.materials.push_back(sphere1Material);
	scene.primitives.spheres.push_back(sphere1);

	// MESH 1 //

	ColorTexture mesh1Texture1;
	mesh1Texture1.id = 3;
	mesh1Texture1.color = Color(1.0, 1.0, 1.0);
	mesh1Texture1.intensity = 0.5;

	ImageTexture mesh1Texture2;
	mesh1Texture2.id = 4;
	mesh1Texture2.imageFilePath = "data/images/test_bumpmap.png";
	mesh1Texture2.applyGamma = false;
	mesh1Texture2.isBumpMap = true;
	mesh1Texture2.intensity = 2.0;

	Material mesh1Material;
	mesh1Material.id = 2;
	mesh1Material.colorTextureId = mesh1Texture1.id;
	mesh1Material.normalMapTextureId = mesh1Texture2.id;
	mesh1Material.diffuseReflectance = Color(0.5, 0.5, 0.5);
	mesh1Material.specularReflectance = Color(1.0, 1.0, 1.0);
	mesh1Material.shininess = 64.0;
	mesh1Material.texcoordScale = Vector2(0.1, 0.1);

	Mesh mesh1;
	mesh1.materialId = mesh1Material.id;
	mesh1.meshFilePath = "data/meshes/square.obj";
	mesh1.position = Vector3(-10.0, 0.0, 10.0);
	mesh1.scale = Vector3(20.0, 20.0, 20.0);
	mesh1.orientation = EulerAngle(-90.0, 0.0, 0.0);

	scene.textures.colorTextures.push_back(mesh1Texture1);
	scene.textures.imageTextures.push_back(mesh1Texture2);
	scene.materials.push_back(mesh1Material);
	scene.primitives.meshes.push_back(mesh1);

	// MESH 2 //

	ImageTexture mesh2Texture1;
	mesh2Texture1.id = 5;
	mesh2Texture1.imageFilePath = "data/images/stonewall.jpg";
	mesh2Texture1.intensity = 1.0;

	ImageTexture mesh2Texture2;
	mesh2Texture2.id = 6;
	mesh2Texture2.imageFilePath = "data/images/stonewall_bumpmap.jpg";
	mesh2Texture2.applyGamma = false;
	mesh2Texture2.isBumpMap = true;
	mesh2Texture2.intensity = 1.0;

	Material mesh2Material;
	mesh2Material.id = 3;
	mesh2Material.colorTextureId = mesh2Texture1.id;
	mesh2Material.normalMapTextureId = mesh2Texture2.id;
	mesh2Material.diffuseReflectance = Color(0.5, 0.5, 0.5);
	mesh2Material.specularReflectance = Color(1.0, 1.0, 1.0);
	mesh2Material.shininess = 64.0;
	mesh2Material.nonShadowing = true;
	mesh2Material.texcoordScale = Vector2(0.25, 0.5);

	Mesh mesh2;
	mesh2.materialId = mesh2Material.id;
	mesh2.meshFilePath = "data/meshes/square.obj";
	mesh2.position = Vector3(-10.0, 0.0, -2.5);
	mesh2.scale = Vector3(20.0, 10.0, 5.0);
	mesh2.orientation = EulerAngle(0.0, 0.0, 0.0);

	scene.textures.imageTextures.push_back(mesh2Texture1);
	scene.textures.imageTextures.push_back(mesh2Texture2);
	scene.materials.push_back(mesh2Material);
	scene.primitives.meshes.push_back(mesh2);

	// MESH 3 //

	ColorTexture mesh3Texture1;
	mesh3Texture1.id = 7;
	mesh3Texture1.color = Color(1.0, 1.0, 1.0);
	mesh3Texture1.intensity = 0.5;

	ImageTexture mesh3Texture2;
	mesh3Texture2.id = 8;
	mesh3Texture2.imageFilePath = "data/images/cube_normalmap.png";
	mesh3Texture2.applyGamma = false;
	mesh3Texture2.isNormalMap = true;
	mesh3Texture2.intensity = 1.0;

	Material mesh3Material;
	mesh3Material.id = 4;
	mesh3Material.colorTextureId = mesh3Texture1.id;
	mesh3Material.normalMapTextureId = mesh3Texture2.id;
	mesh3Material.diffuseReflectance = Color(0.5, 0.5, 0.5);
	mesh3Material.specularReflectance = Color(1.0, 1.0, 1.0);
	mesh3Material.shininess = 64.0;
	mesh3Material.texcoordScale = Vector2(1.0, 1.0);
	mesh3Material.nonShadowing = true;

	Mesh mesh3;
	mesh3.materialId = mesh3Material.id;
	mesh3.meshFilePath = "data/meshes/cube_normalmap.obj";
	mesh3.position = Vector3(-3.0, 1.0, 0.0);
	mesh3.scale = Vector3(0.5, 0.5, 0.5);
	mesh3.orientation = EulerAngle(0.0, 90.0, 30.0);

	scene.textures.colorTextures.push_back(mesh3Texture1);
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

	pointLight1.position = Vector3(-5.0, 5.0, 5.0);
	scene.lights.pointLights.push_back(pointLight1);
	pointLight1.position = Vector3(-5.0, 5.0, -5.0);
	scene.lights.pointLights.push_back(pointLight1);

	return scene;
}
