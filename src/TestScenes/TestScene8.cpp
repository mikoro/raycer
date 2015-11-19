// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// depth of field with a sphere spiral
Scene Scene::createTestScene8()
{
	Scene scene;

	scene.general.cameraSampleCountSqrt = 3;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 0.0, 7.0);
	scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);
	scene.camera.apertureSize = 1.0;
	scene.camera.focalDistance = 30.0;

	// SPHERES //

	Material sphereMaterial;
	sphereMaterial.id = 1;
	sphereMaterial.ambientReflectance = Color(1.0, 1.0, 1.0) * 0.5;
	sphereMaterial.diffuseReflectance = sphereMaterial.ambientReflectance;

	scene.materials.push_back(sphereMaterial);

	double angle = -M_PI / 2.0;
	double radius = 8.0;
	uint64_t currentId = 1;

	for (uint64_t i = 0; i < 50; ++i)
	{
		Sphere sphere;
		sphere.id = currentId;
		sphere.materialId = sphereMaterial.id;
		sphere.position = Vector3(cos(angle) * radius, sin(angle) * radius, i * -1.0);
		sphere.radius = 1.0;

		scene.primitives.spheres.push_back(sphere);
		angle += 0.5;
		++currentId;
	}

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	DirectionalLight directionalLight;
	directionalLight.color = Color(1.0, 1.0, 1.0);
	directionalLight.intensity = 1.0;
	directionalLight.direction = EulerAngle(-10.0, -60.0, 0.0).getDirection();

	scene.lights.directionalLights.push_back(directionalLight);

	return scene;
}
