// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// material preview model (only triangles)
Scene Scene::createTestScene19()
{
	Scene scene;

	scene.general.tracerType = TracerType::RAY;
	scene.general.pathSampleCount = 1;

	scene.rootBVH.enabled = true;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 5.0, 5.0);
	scene.camera.orientation = EulerAngle(-40.0, 0.0, 0.0);

	// GROUND MODEL //

	ImageTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.intensity = 1.0;
	groundTexture.imageFilePath = "data/images/wood_floor.jpg";
	groundTexture.applyGamma = true;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.ambientReflectance = Color(1.0, 1.0, 1.0);
	groundMaterial.diffuseReflectance = Color(1.0, 1.0, 1.0);
	groundMaterial.ambientMapTextureId = groundTexture.id;
	groundMaterial.diffuseMapTextureId = groundTexture.id;
	groundMaterial.texcoordScale = Vector2(50.0, 50.0);

	ModelLoaderInfo modelInfo(ModelLoaderPreset::TRIANGLES);
	modelInfo.modelFilePath = "data/meshes/square.obj";
	modelInfo.defaultMaterialId = groundMaterial.id;
	modelInfo.idStartOffset = 1000;
	modelInfo.scale = Vector3(1000.0, 1000.0, 1000.0);
	modelInfo.rotate = EulerAngle(-90.0, 0.0, 0.0);
	modelInfo.translate = Vector3(-500.0, 0.0, 500.0);

	scene.textures.imageTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.models.push_back(modelInfo);

	// SKY SPHERE MODEL //

	ImageTexture sphereTexture;
	sphereTexture.id = 2;
	sphereTexture.intensity = 1.0;
	sphereTexture.imageFilePath = "data/images/canyon.hdr";

	Material sphereMaterial;
	sphereMaterial.id = 2;
	sphereMaterial.ambientMapTextureId = sphereTexture.id;
	sphereMaterial.diffuseMapTextureId = sphereTexture.id;
	sphereMaterial.emittanceMapTextureId = sphereTexture.id;
	sphereMaterial.skipLighting = true;
	sphereMaterial.nonShadowing = true;
	sphereMaterial.isEmissive = true;

	modelInfo = ModelLoaderInfo(ModelLoaderPreset::TRIANGLES);
	modelInfo.modelFilePath = "data/meshes/sphere.obj";
	modelInfo.defaultMaterialId = sphereMaterial.id;
	modelInfo.idStartOffset = 2000;
	modelInfo.scale = Vector3(1000.0, 1000.0, 1000.0);
	modelInfo.rotate = EulerAngle(0.0, -90.0, 0.0);

	scene.textures.imageTextures.push_back(sphereTexture);
	scene.materials.push_back(sphereMaterial);
	scene.models.push_back(modelInfo);

	// EMITTERS //

	Material emitterMaterial;
	emitterMaterial.id = 3;
	emitterMaterial.ambientReflectance = Color(1.0, 1.0, 1.0);
	emitterMaterial.diffuseReflectance = Color(1.0, 1.0, 1.0);
	emitterMaterial.emittance = Color(1.0, 1.0, 1.0) * 12.0;
	emitterMaterial.nonShadowing = true;
	emitterMaterial.skipLighting = true;
	emitterMaterial.isEmissive = true;

	modelInfo = ModelLoaderInfo(ModelLoaderPreset::TRIANGLES);
	modelInfo.modelFilePath = "data/meshes/square.obj";
	modelInfo.defaultMaterialId = emitterMaterial.id;
	modelInfo.idStartOffset = 3000;
	modelInfo.scale = Vector3(12.0, 12.0, 1.0);
	modelInfo.rotate = EulerAngle(0.0, 30.0, 0.0);
	modelInfo.translate = Vector3(2.0, 5.0, 15.0);

	scene.materials.push_back(emitterMaterial);
	scene.models.push_back(modelInfo);

	modelInfo.idStartOffset = 4000;
	modelInfo.scale = Vector3(8.0, 8.0, 1.0);
	modelInfo.rotate = EulerAngle(0.0, -20.0, 0.0);
	modelInfo.translate = Vector3(-10.0, 5.0, 13.0);

	//scene.models.push_back(modelInfo);

	// MATPREVIEW MODEL //

	Material modelMaterial;
	modelMaterial.id = 4;
	modelMaterial.ambientReflectance = Color(1.0, 1.0, 1.0);
	modelMaterial.diffuseReflectance = Color(1.0, 1.0, 1.0);
	modelMaterial.normalInterpolation = true;

	modelInfo = ModelLoaderInfo(ModelLoaderPreset::TRIANGLES);
	modelInfo.modelFilePath = "data/meshes/matpreview/matpreview.obj";
	//modelInfo.modelFilePath = "data/meshes/sphere.obj";
	modelInfo.defaultMaterialId = modelMaterial.id;
	modelInfo.baseMaterial.normalInterpolation = true;
	modelInfo.idStartOffset = 5000;
	modelInfo.scale = Vector3(1.0, 1.0, 1.0);
	modelInfo.rotate = EulerAngle(0.0, 0.0, 0.0);
	modelInfo.translate = Vector3(0.0, 0.0, 0.0);

	scene.materials.push_back(modelMaterial);
	scene.models.push_back(modelInfo);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.05;

	DirectionalLight directionalLight;
	directionalLight.intensity = 1.0;
	directionalLight.color = Color(1.0, 1.0, 1.0);
	directionalLight.direction = EulerAngle(-25.0, 28.0, 0.0).getDirection();

	scene.lights.directionalLights.push_back(directionalLight);

	return scene;
}
