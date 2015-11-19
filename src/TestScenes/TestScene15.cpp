// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// ambient occlusion with four white spheres and a monkey
Scene Scene::createTestScene15()
{
	Scene scene;

	// CAMERA //

	scene.camera.position = Vector3(2.3, 5.0, 6.0);
	scene.camera.orientation = EulerAngle(-35.0, 0.0, 0.0);

	// GROUND //

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.ambientReflectance = Color(1.0, 1.0, 1.0) * 0.5;
	groundMaterial.diffuseReflectance = groundMaterial.ambientReflectance;

	Plane groundPlane;
	groundPlane.id = 1;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.001, 1.0, 0.001).normalized();

	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// SPHERES //

	Material sphereMaterial;
	sphereMaterial.id = 2;
	sphereMaterial.ambientReflectance = Color(1.0, 1.0, 1.0);
	sphereMaterial.diffuseReflectance = sphereMaterial.ambientReflectance;

	Sphere sphere;
	sphere.materialId = sphereMaterial.id;
	sphere.radius = 1.0;

	scene.materials.push_back(sphereMaterial);

	sphere.id = 2;
	sphere.position = Vector3(-1.0, 1.0, 1.0);
	scene.primitives.spheres.push_back(sphere);
	sphere.id = 3;
	sphere.position = Vector3(1.0, 1.0, 1.0);
	scene.primitives.spheres.push_back(sphere);
	sphere.id = 4;
	sphere.position = Vector3(-1.0, 1.0, -1.0);
	scene.primitives.spheres.push_back(sphere);
	sphere.id = 5;
	sphere.position = Vector3(1.0, 1.0, -1.0);
	scene.primitives.spheres.push_back(sphere);

	// MODEL //

	Material modelMaterial;
	modelMaterial.id = 3;
	modelMaterial.ambientReflectance = Color(1.0, 1.0, 1.0);
	modelMaterial.diffuseReflectance = modelMaterial.ambientReflectance;

	ModelLoaderInfo modelInfo(ModelLoaderPreset::COMBINED_GROUP);
	modelInfo.modelFilePath = "data/meshes/monkey3.obj";
	modelInfo.defaultMaterialId = modelMaterial.id;
	modelInfo.combinedGroupId = 6;
	modelInfo.combinedGroupInstanceId = 7;
	modelInfo.idStartOffset = 8;
	modelInfo.scale = Vector3(5.0, 5.0, 5.0);
	modelInfo.rotate = EulerAngle(-45.0, 0.0, 0.0);
	modelInfo.translate = Vector3(5.0, 0.0, 1.0);

	scene.materials.push_back(modelMaterial);
	scene.models.push_back(modelInfo);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.4;
	scene.lights.ambientLight.enableAmbientOcclusion = true;
	scene.lights.ambientLight.ambientOcclusionMaxSampleDistance = 10.0;
	scene.lights.ambientLight.ambientOcclusionSampleCountSqrt = 3;

	return scene;
}
