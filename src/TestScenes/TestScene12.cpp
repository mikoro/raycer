// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// volumetric fog
Scene Scene::createTestScene12()
{
	Scene scene;

	// CAMERA //

	scene.camera.position = Vector3(4.0, 3.0, 12.0);
	scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);

	scene.volumetricFog.enabled = true;
	scene.volumetricFog.steps = 100;
	scene.volumetricFog.color = Color(1.0, 1.0, 1.0);
	scene.volumetricFog.density = 0.05;

	// GROUND PLANE //

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.ambientReflectance = Color(1.0, 1.0, 1.0);
	groundMaterial.diffuseReflectance = Color(1.0, 1.0, 1.0);

	Plane groundPlane;
	groundPlane.id = 1;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0);

	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// BACK PLANE //

	Plane backPlane;
	backPlane.id = 2;
	backPlane.materialId = groundMaterial.id;
	backPlane.position = Vector3(0.0, 0.0, -20.0);
	backPlane.normal = Vector3(0.0, 0.0, 1.0);

	scene.primitives.planes.push_back(backPlane);

	// SPHERE //

	Material sphereMaterial;
	sphereMaterial.id = 3;
	sphereMaterial.ambientReflectance = Color(1.0, 1.0, 1.0);
	sphereMaterial.diffuseReflectance = Color(1.0, 1.0, 1.0);
	
	Sphere sphere;
	sphere.id = 3;
	sphere.materialId = sphereMaterial.id;
	sphere.position = Vector3(0.0, 5.0, 0.0);
	sphere.radius = 1.0;

	scene.materials.push_back(sphereMaterial);
	scene.primitives.spheres.push_back(sphere);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	/*SpotLight spotLight;
	spotLight.color = Color(1.0, 1.0, 1.0);
	spotLight.intensity = 1.0;
	spotLight.position = Vector3(-10.0, 20.0, 0.0);
	spotLight.maxDistance = 100.0;
	spotLight.attenuation = 1.0;
	spotLight.direction = EulerAngle(-55.0, -90.0, 0.0).getDirection();
	spotLight.angle = 2.0;
	spotLight.sideAttenuation = 1.0;
	spotLight.enableAreaLight = true;
	spotLight.areaLightSamplerType = SamplerType::CMJ;
	spotLight.areaLightSampleCountSqrt = 3;
	spotLight.areaLightRadius = 1.0;

	scene.lights.spotLights.push_back(spotLight);*/
	
	PointLight pointLight;
	pointLight.color = Color(1.0, 1.0, 1.0);
	pointLight.intensity = 1.0;
	pointLight.position = Vector3(-10.0, 20.0, 0.0);
	pointLight.maxDistance = 40.0;
	pointLight.attenuation = 1.0;
	pointLight.enableAreaLight = false;
	pointLight.areaLightSamplerType = SamplerType::CMJ;
	pointLight.areaLightSampleCountSqrt = 3;
	pointLight.areaLightRadius = 1.0;

	scene.lights.pointLights.push_back(pointLight);

	return scene;
}
