// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// glossy reflections
Scene Scene::createTestScene11()
{
	Scene scene;

	scene.toneMapper.type = ToneMapperType::SIMPLE;

	// CAMERA //

	scene.camera.position = Vector3(0.44, 7.05, 7.33);
	scene.camera.orientation = EulerAngle(-21.89, 8.77, 0.0);

	// BOUNDING SPHERE //

	ColorGradientTexture sphereTexture;
	sphereTexture.id = 1;
	sphereTexture.hasVerticalColorGradient = true;
	sphereTexture.verticalColorGradient.addSegment(Color(0.0, 0.0, 0.0), Color(0.0, 0.0, 0.0), 40);
	sphereTexture.verticalColorGradient.addSegment(Color(0.0, 0.0, 0.0), Color(0.0, 0.0, 0.05), 60);
	sphereTexture.verticalIntensity = 1.0;

	Material sphereMaterial;
	sphereMaterial.id = 1;
	sphereMaterial.diffuseReflectance = Color(1.0, 1.0, 1.0);
	sphereMaterial.diffuseMapTextureId = sphereTexture.id;
	sphereMaterial.skipLighting = true;
	sphereMaterial.invertNormal = true;

	Sphere sphere;
	sphere.id = 1;
	sphere.materialId = sphereMaterial.id;
	sphere.position = Vector3(0.0, 0.0, 0.0);
	sphere.radius = 1000.0;

	scene.textures.colorGradientTextures.push_back(sphereTexture);
	scene.materials.push_back(sphereMaterial);
	scene.primitives.spheres.push_back(sphere);

	// GROUND PLANE //

	CheckerTexture groundTexture;
	groundTexture.id = 2;
	groundTexture.stripeMode = true;
	groundTexture.stripeWidth = 0.05;
	groundTexture.color1 = Color(0.025, 0.0, 0.0);
	groundTexture.color2 = Color(0.0, 0.0, 0.0);

	Material groundMaterial;
	groundMaterial.id = 2;
	groundMaterial.ambientReflectance = Color(1.0, 1.0, 1.0);
	groundMaterial.diffuseReflectance = Color(1.0, 1.0, 1.0);
	groundMaterial.ambientMapTextureId = groundTexture.id;
	groundMaterial.diffuseMapTextureId = groundTexture.id;
	groundMaterial.texcoordScale = Vector2(0.5, 0.5);
	groundMaterial.rayReflectance = 0.1;

	Plane groundPlane;
	groundPlane.id = 2;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0);

	scene.textures.checkerTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// BOXES //

	Material boxMaterial;
	Box box;
	Instance boxInstance;

	boxMaterial.id = 10;
	boxMaterial.ambientReflectance = Color(0.0, 0.0, 0.0);
	boxMaterial.diffuseReflectance = Color(0.0, 0.0, 0.0);
	boxMaterial.rayTransmittance = 1.0;
	boxMaterial.refractiveIndex = 1.5;
	boxMaterial.rayTransmittanceGlossinessSampleCountSqrt = 2;
	boxMaterial.rayTransmittanceGlossiness = 1000.0;
	boxMaterial.nonShadowing = true;
	box.id = 10;
	box.invisible = true;
	box.materialId = boxMaterial.id;
	box.position = Vector3(0.0, 3.01, -1.0);
	box.extent = Vector3(4.0, 6.0, 1.0);
	boxInstance.id = 1000;
	boxInstance.primitiveId = box.id;
	boxInstance.rotate = EulerAngle(0.0, 20.0, 0.0);
	scene.materials.push_back(boxMaterial);
	scene.primitives.boxes.push_back(box);
	scene.primitives.instances.push_back(boxInstance);

	boxMaterial.id = 12;
	boxMaterial.ambientReflectance = Color(1.0, 1.0, 1.0);
	boxMaterial.diffuseReflectance = Color(1.0, 1.0, 1.0);
	boxMaterial.rayTransmittance = 0.0;
	box.id = 12;
	box.invisible = true;
	box.materialId = boxMaterial.id;
	box.position = Vector3(-1.0, 2.0, -6.0);
	box.extent = Vector3(14.0, 4.0, 0.5);
	boxInstance.id = 1002;
	boxInstance.primitiveId = box.id;
	boxInstance.rotate = EulerAngle(0.0, -20.0, 0.0);
	scene.materials.push_back(boxMaterial);
	scene.primitives.boxes.push_back(box);
	scene.primitives.instances.push_back(boxInstance);

	// SPHERES //

	sphereMaterial = Material();
	sphereMaterial.id = 20;
	sphereMaterial.ambientReflectance = Color(1.0, 0.0, 0.0);
	sphereMaterial.diffuseReflectance = Color(1.0, 0.0, 0.0);
	sphereMaterial.nonShadowing = true;

	sphere = Sphere();
	sphere.id = 20;
	sphere.materialId = sphereMaterial.id;
	sphere.position = Vector3(-2.5, 1.5, -2.3);
	sphere.radius = 1.5;

	scene.materials.push_back(sphereMaterial);
	scene.primitives.spheres.push_back(sphere);

	sphereMaterial = Material();
	sphereMaterial.id = 21;
	sphereMaterial.ambientReflectance = Color(0.01, 0.01, 0.01);
	sphereMaterial.diffuseReflectance = Color(0.01, 0.01, 0.01);
	sphereMaterial.rayReflectance = 1.0;
	sphereMaterial.rayReflectanceGlossinessSampleCountSqrt = 2;
	sphereMaterial.rayReflectanceGlossiness = 50.0;
	sphereMaterial.nonShadowing = true;

	sphere = Sphere();
	sphere.id = 21;
	sphere.materialId = sphereMaterial.id;
	sphere.position = Vector3(-8.0, 2.0, -6.0);
	sphere.radius = 2.0;

	scene.materials.push_back(sphereMaterial);
	scene.primitives.spheres.push_back(sphere);
	
	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	PointLight pointLight;
	pointLight.color = Color(1.0, 1.0, 1.0);
	pointLight.intensity = 2.0;
	pointLight.position = Vector3(10.0, 10.0, 10.0);
	pointLight.maxDistance = 1000.0;
	pointLight.attenuation = 2.0;
	
	scene.lights.pointLights.push_back(pointLight);

	return scene;
}
