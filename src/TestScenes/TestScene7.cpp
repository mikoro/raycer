// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// reflection and refraction with boxes
Scene Scene::createTestScene7()
{
	Scene scene;

	scene.general.backgroundColor = Color(0.8, 0.8, 0.8);
	scene.general.maxRayIterations = 12;

	// CAMERA //

	scene.camera.position = Vector3(9.49, 4.44, 3.94);
	scene.camera.orientation = EulerAngle(-20.0, 35.0, 0.0);

	// BOXES //

	Material boxMaterial;
	boxMaterial.id = 1;
	boxMaterial.ambientReflectance = Color(0.5, 0.0, 0.0);
	boxMaterial.diffuseReflectance = Color(0.5, 0.0, 0.0);
	boxMaterial.rayTransmittance = 0.75;
	boxMaterial.refractiveIndex = 1.0;
	boxMaterial.enableRayTransmissionAttenuation = true;
	boxMaterial.rayTransmissionAttenuationFactor = 1.0;
	boxMaterial.rayTransmissionAttenuationColor = Color(0.1, 0.0, 0.0);
	boxMaterial.nonShadowing = true;

	scene.materials.push_back(boxMaterial);

	Box box;
	box.extent = Vector3(2.0, 2.0, 2.0);
	box.materialId = boxMaterial.id;

	box.id = 1;
	box.position = Vector3(-4.0, 1.0, 0.0);
	scene.primitives.boxes.push_back(box);
	box.id = 2;
	box.position = Vector3(-8.0, 1.0, 0.0);
	scene.primitives.boxes.push_back(box);

	// MODEL BOXES //

	ModelLoaderInfo modelInfo(ModelLoaderPreset::COMBINED_GROUP);
	modelInfo.modelFilePath = "data/meshes/cube1.obj";
	modelInfo.defaultMaterialId = boxMaterial.id;

	modelInfo.translate = Vector3(4.0, 1.0, 0.0);
	modelInfo.combinedGroupId = 6;
	modelInfo.combinedGroupInstanceId = 7;
	modelInfo.idStartOffset = 1000;
	scene.models.push_back(modelInfo);

	modelInfo.translate = Vector3(8.0, 1.0, 0.0);
	modelInfo.combinedGroupId = 8;
	modelInfo.combinedGroupInstanceId = 9;
	modelInfo.idStartOffset = 2000;
	scene.models.push_back(modelInfo);

	// WALL BOXES //

	boxMaterial = Material();
	boxMaterial.id = 2;
	boxMaterial.ambientReflectance = Color(0.1, 0.1, 0.3);
	boxMaterial.diffuseReflectance = Color(0.0, 0.0, 0.0);
	boxMaterial.rayReflectance = 0.75;
	boxMaterial.nonShadowing = true;

	box = Box();
	box.id = 10;
	box.materialId = boxMaterial.id;
	box.invisible = true;
	box.extent = Vector3(20.0, 10.0, 1.0);
	box.position = Vector3(0.0, 0.0, 0.0);

	scene.materials.push_back(boxMaterial);
	scene.primitives.boxes.push_back(box);

	// WALL BOX INSTANCES //

	Instance instance;
	instance.primitiveId = box.id;

	instance.id = 11;
	instance.translate = Vector3(0.0, 0.0, -5.0);
	instance.rotate = EulerAngle(0.0, 0.0, 0.0);
	scene.primitives.instances.push_back(instance);

	instance.id = 12;
	instance.translate = Vector3(0.0, 0.0, 5.0);
	instance.rotate = EulerAngle(0.0, 0.0, 0.0);
	scene.primitives.instances.push_back(instance);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	DirectionalLight directionalLight;
	directionalLight.color = Color(1.0, 1.0, 1.0);
	directionalLight.intensity = 1.0;
	directionalLight.direction = EulerAngle(-10.0, 30.0, 0.0).getDirection();

	scene.lights.directionalLights.push_back(directionalLight);

	return scene;
}
