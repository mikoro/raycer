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
	sphere1Material.ambientMapTextureId = sphere1Texture1.id;
	sphere1Material.diffuseMapTextureId = sphere1Texture1.id;
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
	mesh1Material.ambientMapTextureId = mesh1Texture1.id;
	mesh1Material.diffuseMapTextureId = mesh1Texture1.id;
	mesh1Material.normalMapTextureId = mesh1Texture2.id;
	mesh1Material.diffuseReflectance = Color(0.5, 0.5, 0.5);
	mesh1Material.specularReflectance = Color(1.0, 1.0, 1.0);
	mesh1Material.shininess = 64.0;
	mesh1Material.texcoordScale = Vector2(0.1, 0.1);

	ModelLoaderInfo modelInfo1;
	modelInfo1.modelFilePath = "data/meshes/square.obj";
	modelInfo1.defaultMaterialId = mesh1Material.id;
	modelInfo1.idStartOffset = 1000;
	modelInfo1.scale = Vector3(20.0, 20.0, 20.0);
	modelInfo1.rotate = EulerAngle(-90.0, 0.0, 0.0);
	modelInfo1.translate = Vector3(-10.0, 0.0, 10.0);

	scene.textures.colorTextures.push_back(mesh1Texture1);
	scene.textures.imageTextures.push_back(mesh1Texture2);
	scene.materials.push_back(mesh1Material);
	scene.models.push_back(modelInfo1);

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
	mesh2Material.ambientMapTextureId = mesh2Texture1.id;
	mesh2Material.diffuseMapTextureId = mesh2Texture1.id;
	mesh2Material.normalMapTextureId = mesh2Texture2.id;
	mesh2Material.diffuseReflectance = Color(0.5, 0.5, 0.5);
	mesh2Material.specularReflectance = Color(1.0, 1.0, 1.0);
	mesh2Material.shininess = 64.0;
	mesh2Material.nonShadowing = true;
	mesh2Material.texcoordScale = Vector2(0.25, 0.5);

	ModelLoaderInfo modelInfo2;
	modelInfo2.modelFilePath = "data/meshes/square.obj";
	modelInfo2.defaultMaterialId = mesh2Material.id;
	modelInfo2.idStartOffset = 2000;
	modelInfo2.scale = Vector3(20.0, 10.0, 5.0);
	modelInfo2.rotate = EulerAngle(0.0, 0.0, 0.0);
	modelInfo2.translate = Vector3(-10.0, 0.0, -2.5);

	scene.textures.imageTextures.push_back(mesh2Texture1);
	scene.textures.imageTextures.push_back(mesh2Texture2);
	scene.materials.push_back(mesh2Material);
	scene.models.push_back(modelInfo2);

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
	mesh3Material.ambientMapTextureId = mesh3Texture1.id;
	mesh3Material.diffuseMapTextureId = mesh3Texture1.id;
	mesh3Material.normalMapTextureId = mesh3Texture2.id;
	mesh3Material.diffuseReflectance = Color(0.5, 0.5, 0.5);
	mesh3Material.specularReflectance = Color(1.0, 1.0, 1.0);
	mesh3Material.shininess = 64.0;
	mesh3Material.texcoordScale = Vector2(1.0, 1.0);
	mesh3Material.nonShadowing = true;

	ModelLoaderInfo modelInfo3;
	modelInfo3.modelFilePath = "data/meshes/cube_normalmap.obj";
	modelInfo3.defaultMaterialId = mesh3Material.id;
	modelInfo3.idStartOffset = 3000;
	modelInfo3.scale = Vector3(0.5, 0.5, 0.5);
	modelInfo3.rotate = EulerAngle(0.0, 90.0, 30.0);
	modelInfo3.translate = Vector3(-3.0, 1.0, 0.0);

	scene.textures.colorTextures.push_back(mesh3Texture1);
	scene.textures.imageTextures.push_back(mesh3Texture2);
	scene.materials.push_back(mesh3Material);
	scene.models.push_back(modelInfo3);

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
