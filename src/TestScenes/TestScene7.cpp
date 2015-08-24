// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// depth of field with a sphere spiral
Scene Scene::createTestScene7()
{
	Scene scene;

	scene.raytracer.dofSamples = 10;

	scene.raytracer.multiSamplerType = SamplerType::CMJ;
	scene.raytracer.multiSamplerFilterType = FilterType::CUBIC_BSPLINE;
	scene.raytracer.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 0.0, 7.0);
	scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);
	scene.camera.apertureSize = 0.5;
	scene.camera.focalLenght = 30.0;
	
	// SPHERES //

	ColorTexture sphereTexture;
	sphereTexture.id = 2;
	sphereTexture.color = Color(1.0, 1.0, 1.0);
	sphereTexture.intensity = 0.5;

	Material sphereMaterial;
	sphereMaterial.id = 2;
	sphereMaterial.colorTextureId = sphereTexture.id;

	scene.textures.colorTextures.push_back(sphereTexture);
	scene.materials.push_back(sphereMaterial);

	double angle = -M_PI / 2.0;
	double radius = 8.0;
	
	for (int i = 0; i < 50; ++i)
	{
		Sphere sphere;
		sphere.position = Vector3(cos(angle) * radius, sin(angle) * radius, i * -1.0);
		sphere.radius = 1.0;
		sphere.materialId = sphereMaterial.id;
		scene.primitives.spheres.push_back(sphere);
		angle += 0.5;
	}

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;
	
	DirectionalLight directionalLight1;
	directionalLight1.color = Color(1.0, 1.0, 1.0);
	directionalLight1.intensity = 1.0;
	directionalLight1.direction = EulerAngle(-10.0, -60.0, 0.0).getDirection();

	scene.lights.directionalLights.push_back(directionalLight1);

	return scene;
}
