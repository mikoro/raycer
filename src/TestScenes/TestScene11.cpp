// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// soft shadows and glossy reflections
Scene Scene::createTestScene11()
{
	Scene scene;

	scene.toneMapper.type = ToneMapperType::SIMPLE;

	// CAMERA //

	scene.camera.position = Vector3(0.44, 7.05, 7.33);
	scene.camera.orientation = EulerAngle(-26.56, 22.22, 0.0);

	// GROUND //

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.ambientReflectance = Color(1.0, 1.0, 1.0) * 0.0;
	groundMaterial.diffuseReflectance = Color(1.0, 1.0, 1.0) * 0.5;
	groundMaterial.rayReflectance = 1.0;
	//groundMaterial.rayReflectanceGlossinessSamplesSqrt = 3;
	groundMaterial.rayReflectanceGlossiness = 100.0;

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

	// SPHERES //

	Material sphereMaterial;
	sphereMaterial.id = 10;
	sphereMaterial.ambientReflectance = Color(0.0, 1.0, 0.0) * 0.0;
	sphereMaterial.diffuseReflectance = Color(0.0, 1.0, 0.0) * 0.0;
	sphereMaterial.rayReflectance = 1.0;
	sphereMaterial.rayReflectanceGlossinessSamplesSqrt = 3;
	sphereMaterial.rayReflectanceGlossiness = 100.0;

	Sphere sphere;
	sphere.id = 10;
	sphere.materialId = sphereMaterial.id;
	sphere.position = Vector3(3.0, 1.0, 3.0);
	sphere.radius = 1.0;

	scene.materials.push_back(sphereMaterial);
	scene.primitives.spheres.push_back(sphere);
	
	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	PointLight pointLight;
	pointLight.color = Color(1.0, 1.0, 1.0);
	pointLight.intensity = 2.0;
	pointLight.position = Vector3(10.0, 10.0, 10.0);
	pointLight.maxDistance = 100.0;
	pointLight.attenuation = 1.0;
	//pointLight.enableAreaLight = true;
	pointLight.areaLightSamplesSqrt = 3;
	pointLight.areaLightRadius = 0.5;

	scene.lights.pointLights.push_back(pointLight);

	return scene;
}
