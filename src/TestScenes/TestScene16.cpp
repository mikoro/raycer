// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// soft shadows with boxes
Scene Scene::createTestScene16()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(0.44, 7.05, 7.33);
	scene.camera.orientation = EulerAngle(-26.56, 22.22, 0.0);

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

	// BOXES //

	Material boxMaterial;
	boxMaterial.id = 2;
	boxMaterial.ambientReflectance = Color(0.2, 0.2, 1.0) * 0.5;
	boxMaterial.diffuseReflectance = boxMaterial.ambientReflectance;

	scene.materials.push_back(boxMaterial);

	Box box;
	box.materialId = boxMaterial.id;

	box.id = 2;
	box.position = Vector3(0.0, 3.0, 0.0);
	box.extent = Vector3(2.0, 6.0, 2.0);
	scene.primitives.boxes.push_back(box);
	box.id = 3;
	box.position = Vector3(-5.0, 0.5, -2.5);
	box.extent = Vector3(2.0, 1.0, 5.0);
	scene.primitives.boxes.push_back(box);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	PointLight pointLight;
	pointLight.color = Color(1.0, 1.0, 1.0);
	pointLight.intensity = 1.5;
	pointLight.position = Vector3(10.0, 10.0, 10.0);
	pointLight.distance = 100.0;
	pointLight.attenuation = 1.0;
	pointLight.softShadows = true;
	pointLight.samples = 3;
	pointLight.radius = 0.5;

	scene.lights.pointLights.push_back(pointLight);

	return scene;
}
