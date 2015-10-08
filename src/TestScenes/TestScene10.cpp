// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// bump/normal mapping
Scene Scene::createTestScene10()
{
	Scene scene;

	scene.general.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(-0.18, 1.65, 3.59);
	scene.camera.orientation = EulerAngle(-9.22, 17.56, 0.0);

	// SPHERE //

	PerlinNoiseTexture sphereNormalTexture;
	sphereNormalTexture.id = 1;
	sphereNormalTexture.seed = 2345654;
	sphereNormalTexture.intensity = 0.05;
	sphereNormalTexture.scale = Vector3(10.0, 10.0, 10.0);
	sphereNormalTexture.isFbm = true;

	Material sphereMaterial;
	sphereMaterial.id = 1;
	sphereMaterial.ambientReflectance = Color(1.0, 0.1, 0.1) * 0.5;
	sphereMaterial.diffuseReflectance = sphereMaterial.ambientReflectance;
	sphereMaterial.specularReflectance = Color(1.0, 1.0, 1.0);
	sphereMaterial.normalMapTextureId = sphereNormalTexture.id;
	sphereMaterial.shininess = 64.0;
	sphereMaterial.nonShadowing = true;

	Sphere sphere;
	sphere.id = 1;
	sphere.materialId = sphereMaterial.id;
	sphere.position = Vector3(0.0, 1.0, 0.0);
	sphere.radius = 1.0;

	scene.textures.perlinNoiseTextures.push_back(sphereNormalTexture);
	scene.materials.push_back(sphereMaterial);
	scene.primitives.spheres.push_back(sphere);

	// FLOOR MODEL //

	ImageTexture model1NormalTexture;
	model1NormalTexture.id = 2;
	model1NormalTexture.imageFilePath = "data/images/test_bumpmap.png";
	model1NormalTexture.isBumpMap = true;
	model1NormalTexture.intensity = 2.0;

	Material model1Material;
	model1Material.id = 2;
	model1Material.ambientReflectance = Color(1.0, 1.0, 1.0) * 0.5;
	model1Material.diffuseReflectance = model1Material.ambientReflectance;
	model1Material.specularReflectance = Color(1.0, 1.0, 1.0);
	model1Material.normalMapTextureId = model1NormalTexture.id;
	model1Material.shininess = 64.0;
	model1Material.texcoordScale = Vector2(10.0, 10.0);

	ModelLoaderInfo model1Info(ModelLoaderPreset::COMBINED_GROUP);
	model1Info.modelFilePath = "data/meshes/square.obj";
	model1Info.defaultMaterialId = model1Material.id;
	model1Info.combinedGroupId = 2;
	model1Info.combinedGroupInstanceId = 3;
	model1Info.idStartOffset = 1000;
	model1Info.scale = Vector3(20.0, 20.0, 20.0);
	model1Info.rotate = EulerAngle(-90.0, 0.0, 0.0);
	model1Info.translate = Vector3(-10.0, 0.0, 10.0);

	scene.textures.imageTextures.push_back(model1NormalTexture);
	scene.materials.push_back(model1Material);
	scene.models.push_back(model1Info);

	// WALL MODEL //

	ImageTexture model2DiffuseTexture;
	model2DiffuseTexture.id = 3;
	model2DiffuseTexture.imageFilePath = "data/images/stonewall_diffuse.png";
	model2DiffuseTexture.intensity = 1.0;
	model2DiffuseTexture.applyGamma = true;

	ImageTexture model2SpecularTexture;
	model2SpecularTexture.id = 4;
	model2SpecularTexture.imageFilePath = "data/images/stonewall_specular.png";
	model2SpecularTexture.intensity = 1.0;
	model2SpecularTexture.applyGamma = true;

	ImageTexture model2NormalTexture;
	model2NormalTexture.id = 5;
	model2NormalTexture.imageFilePath = "data/images/stonewall_normal.png";
	model2NormalTexture.isNormalMap = true;

	Material model2Material;
	model2Material.id = 3;
	model2Material.ambientMapTextureId = model2DiffuseTexture.id;
	model2Material.diffuseMapTextureId = model2DiffuseTexture.id;
	model2Material.specularMapTextureId = model2SpecularTexture.id;
	model2Material.normalMapTextureId = model2NormalTexture.id;
	model2Material.diffuseReflectance = Color(1.0, 1.0, 1.0) * 0.5;
	model2Material.specularReflectance = Color(1.0, 1.0, 1.0);
	model2Material.shininess = 64.0;
	model2Material.nonShadowing = true;
	model2Material.texcoordScale = Vector2(4.0, 2.0);

	ModelLoaderInfo model2Info(ModelLoaderPreset::COMBINED_GROUP);
	model2Info.modelFilePath = "data/meshes/square.obj";
	model2Info.defaultMaterialId = model2Material.id;
	model2Info.combinedGroupId = 4;
	model2Info.combinedGroupInstanceId = 5;
	model2Info.idStartOffset = 2000;
	model2Info.scale = Vector3(20.0, 10.0, 5.0);
	model2Info.rotate = EulerAngle(0.0, 0.0, 0.0);
	model2Info.translate = Vector3(-10.0, 0.0, -2.5);

	scene.textures.imageTextures.push_back(model2DiffuseTexture);
	scene.textures.imageTextures.push_back(model2SpecularTexture);
	scene.textures.imageTextures.push_back(model2NormalTexture);
	scene.materials.push_back(model2Material);
	scene.models.push_back(model2Info);

	// CUBE MODEL //

	ImageTexture model3NormalTexture;
	model3NormalTexture.id = 6;
	model3NormalTexture.imageFilePath = "data/images/cube_normalmap.png";
	model3NormalTexture.isNormalMap = true;

	Material mesh3Material;
	mesh3Material.id = 4;
	mesh3Material.ambientReflectance = Color(1.0, 1.0, 1.0) * 0.25;
	mesh3Material.diffuseReflectance = mesh3Material.ambientReflectance;
	mesh3Material.specularReflectance = Color(1.0, 1.0, 1.0);
	mesh3Material.normalMapTextureId = model3NormalTexture.id;
	mesh3Material.shininess = 64.0;
	mesh3Material.nonShadowing = true;

	ModelLoaderInfo model3Info(ModelLoaderPreset::COMBINED_GROUP);
	model3Info.modelFilePath = "data/meshes/cube_normalmap.obj";
	model3Info.defaultMaterialId = mesh3Material.id;
	model3Info.combinedGroupId = 6;
	model3Info.combinedGroupInstanceId = 7;
	model3Info.idStartOffset = 3000;
	model3Info.scale = Vector3(0.5, 0.5, 0.5);
	model3Info.rotate = EulerAngle(-42.59, 161.79, 0.0);
	model3Info.translate = Vector3(-3.0, 1.0, 0.0);

	scene.textures.imageTextures.push_back(model3NormalTexture);
	scene.materials.push_back(mesh3Material);
	scene.models.push_back(model3Info);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.01;

	PointLight pointLight;
	pointLight.color = Color(1.0, 1.0, 1.0);
	pointLight.intensity = 0.6;
	pointLight.maxDistance = 20.0;
	pointLight.attenuation = 1.0;

	pointLight.position = Vector3(-5.0, 5.0, 5.0);
	scene.lights.pointLights.push_back(pointLight);
	pointLight.position = Vector3(-5.0, 5.0, -5.0);
	scene.lights.pointLights.push_back(pointLight);

	return scene;
}
