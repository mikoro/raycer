// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// spherical environment texture map with glass primitives
Scene Scene::createTestScene6()
{
	Scene scene;

	scene.raytracer.maxRayIterations = 8;
	scene.raytracer.multiSamples = 0;

	scene.toneMapper.type = ToneMapperType::REINHARD;

	// CAMERA //

	scene.camera.position = Vector3(7.0, 0.0, -12.2);
	scene.camera.orientation = EulerAngle(0.0, 150.0, 0.0);

	// SKY SPHERE //

	ImageTexture sphereTexture;
	sphereTexture.id = 1;
	sphereTexture.intensity = 1.0;
	sphereTexture.imageFilePath = "data/images/rooftop.hdr";
	sphereTexture.applyGamma = false;

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
	sphereMaterial.ambientReflectance = Color(1.0, 1.0, 1.0);
	sphereMaterial.diffuseReflectance = sphereMaterial.ambientReflectance;
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
	sphereMaterial.ambientReflectance = Color(1.0, 1.0, 1.0);
	sphereMaterial.diffuseReflectance = sphereMaterial.ambientReflectance;
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
	boxMaterial.ambientReflectance = Color(1.0, 1.0, 1.0);
	boxMaterial.diffuseReflectance = boxMaterial.ambientReflectance;
	boxMaterial.rayReflectance = 1.0;
	boxMaterial.rayTransmittance = 1.0;
	boxMaterial.refractiveIndex = 1.5;
	boxMaterial.fresnelReflection = true;
	boxMaterial.enableAttenuation = true;
	boxMaterial.attenuationFactor = 0.4;
	boxMaterial.attenuationColor = Color(0.0, 0.0, 0.0);

	Box box;
	box.id = 4;
	box.materialId = boxMaterial.id;
	box.position = Vector3(0.0, 0.0, 0.0);
	box.extent = Vector3(1.0, 10.0, 6.0);

	scene.materials.push_back(boxMaterial);
	scene.primitives.boxes.push_back(box);

	return scene;
}
