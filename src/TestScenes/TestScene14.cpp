﻿// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// spherical environment texture map with glass primitives
Scene Scene::createTestScene14()
{
	Scene scene;

	scene.general.maxRayIterations = 8;
	
	scene.toneMapper.type = ToneMapperType::REINHARD;
	scene.toneMapper.enableAveraging = true;
	scene.toneMapper.averagingAlpha = 0.1;

	// CAMERA //

	scene.camera.position = Vector3(7.0, 0.0, -12.2);
	scene.camera.orientation = EulerAngle(0.0, 150.0, 0.0);

	// SKY SPHERE //

	ImageTexture sphereTexture;
	sphereTexture.id = 1;
	sphereTexture.intensity = 1.0;
	sphereTexture.imageFilePath = "data/images/rooftop.hdr";

	Material sphereMaterial;
	sphereMaterial.id = 1;
	sphereMaterial.ambientMapTextureId = sphereTexture.id;
	sphereMaterial.diffuseMapTextureId = sphereTexture.id;
	sphereMaterial.skipLighting = true;
	sphereMaterial.texcoordScale = Vector2(-1.0, 1.0);

	Sphere sphere;
	sphere.id = 1;
	sphere.materialId = sphereMaterial.id;
	sphere.position = Vector3(0.0, 0.0, 0.0);
	sphere.radius = 100.0;

	scene.textures.imageTextures.push_back(sphereTexture);
	scene.materials.push_back(sphereMaterial);
	scene.primitives.spheres.push_back(sphere);

	// SMALL SPHERE 1 //

	sphereMaterial = Material();
	sphereMaterial.id = 2;
	sphereMaterial.ambientReflectance = Color(1.0, 1.0, 1.0) * 0.0;
	sphereMaterial.diffuseReflectance = Color(1.0, 1.0, 1.0) * 0.0;
	sphereMaterial.rayReflectance = 1.0;
	sphereMaterial.rayTransmittance = 1.0;
	sphereMaterial.refractiveIndex = 1.5;
	sphereMaterial.fresnelReflection = true;

	sphere = Sphere();
	sphere.id = 2;
	sphere.materialId = sphereMaterial.id;
	sphere.position = Vector3(4.0, -2.0, 0.0);
	sphere.radius = 2.0;

	scene.materials.push_back(sphereMaterial);
	scene.primitives.spheres.push_back(sphere);

	// SMALL SPHERE 2 //

	sphereMaterial = Material();
	sphereMaterial.id = 3;
	sphereMaterial.ambientReflectance = Color(1.0, 1.0, 1.0) * 0.0;
	sphereMaterial.diffuseReflectance = Color(1.0, 1.0, 1.0) * 0.0;
	sphereMaterial.rayReflectance = 1.0;
	sphereMaterial.rayTransmittance = 1.0;
	sphereMaterial.refractiveIndex = 1.5;
	sphereMaterial.fresnelReflection = true;

	sphere = Sphere();
	sphere.id = 3;
	sphere.materialId = sphereMaterial.id;
	sphere.position = Vector3(-4.0, 2.0, 0.0);
	sphere.radius = 2.0;

	scene.materials.push_back(sphereMaterial);
	scene.primitives.spheres.push_back(sphere);

	// BOX //

	Material boxMaterial;
	boxMaterial.id = 4;
	boxMaterial.ambientReflectance = Color(1.0, 1.0, 1.0) * 0.0;
	boxMaterial.diffuseReflectance = Color(1.0, 1.0, 1.0) * 0.0;
	boxMaterial.rayReflectance = 1.0;
	boxMaterial.rayTransmittance = 1.0;
	boxMaterial.refractiveIndex = 1.5;
	boxMaterial.fresnelReflection = true;
	boxMaterial.enableRayTransmissionAttenuation = true;
	boxMaterial.rayTransmissionAttenuationFactor = 0.4;
	boxMaterial.rayTransmissionAttenuationColor = Color(0.0, 0.0, 0.0);

	Box box;
	box.id = 4;
	box.materialId = boxMaterial.id;
	box.position = Vector3(0.0, 0.0, 0.0);
	box.extent = Vector3(1.0, 10.0, 6.0);

	scene.materials.push_back(boxMaterial);
	scene.primitives.boxes.push_back(box);

	// LIGHTS //

	scene.lights.ambientLight.intensity = 0.0;

	return scene;
}
