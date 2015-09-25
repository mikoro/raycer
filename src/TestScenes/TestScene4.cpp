// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// monkey mesh on a plane with two colored point lights
Scene Scene::createTestScene4()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 5.0, 8.0);
	scene.camera.orientation = EulerAngle(-26.0, 0.0, 0.0);

	// GROUND //

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.ambientReflectance = Color(1.0, 1.0, 1.0) * 0.3;
	groundMaterial.diffuseReflectance = groundMaterial.ambientReflectance;

	Plane groundPlane;
	groundPlane.id = 1;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();

	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// MODEL //

	Material modelMaterial;
	modelMaterial.id = 2;
	modelMaterial.ambientReflectance = Color(1.0, 1.0, 1.0) * 0.8;
	modelMaterial.diffuseReflectance = modelMaterial.ambientReflectance;

	ModelLoaderInfo modelInfo;
	modelInfo.modelFilePath = "data/meshes/monkey3.obj";
	modelInfo.defaultMaterialId = modelMaterial.id;
	modelInfo.combinedGroupId = 2;
	modelInfo.combinedGroupInstanceId = 3;
	modelInfo.scale = Vector3(6.0, 6.0, 6.0);

	scene.materials.push_back(modelMaterial);
	scene.models.push_back(modelInfo);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	PointLight pointLight1;
	pointLight1.color = Color(1.0, 0.25, 0.25);
	pointLight1.intensity = 1.5;
	pointLight1.position = Vector3(5.0, 5.0, 5.0);
	pointLight1.distance = 20.0;
	pointLight1.attenuation = 1.0;

	PointLight pointLight2;
	pointLight2.color = Color(0.25, 0.25, 1.0);
	pointLight2.intensity = 1.5;
	pointLight2.position = Vector3(-5.0, 5.0, 5.0);
	pointLight2.distance = 20.0;
	pointLight2.attenuation = 1.0;

	scene.lights.pointLights.push_back(pointLight1);
	scene.lights.pointLights.push_back(pointLight2);

	return scene;
}
