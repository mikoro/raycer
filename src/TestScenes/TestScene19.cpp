// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// material preview model (only triangles)
Scene Scene::createTestScene19()
{
	Scene scene;

	//scene.general.tracerType = TracerType::PATH;
	//scene.toneMapper.type = ToneMapperType::REINHARD;

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
	sphereMaterial.skipLighting = true;
	sphereMaterial.nonShadowing = true;

	modelInfo = ModelLoaderInfo(ModelLoaderPreset::TRIANGLES);
	modelInfo.modelFilePath = "data/meshes/sphere.obj";
	modelInfo.defaultMaterialId = sphereMaterial.id;
	modelInfo.idStartOffset = 2000;
	modelInfo.scale = Vector3(1000.0, 1000.0, 1000.0);
	modelInfo.rotate = EulerAngle(0.0, -90.0, 0.0);

	scene.textures.imageTextures.push_back(sphereTexture);
	scene.materials.push_back(sphereMaterial);
	scene.models.push_back(modelInfo);

	// MATPREVIEW MODEL //

	modelInfo = ModelLoaderInfo(ModelLoaderPreset::TRIANGLES);
	modelInfo.modelFilePath = "data/meshes/matpreview/matpreview.obj";
	modelInfo.idStartOffset = 3000;
	modelInfo.baseMaterial.normalInterpolation = true;

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
