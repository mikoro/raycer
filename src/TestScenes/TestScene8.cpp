// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <random>

#include "Raytracing/Scene.h"

using namespace Raycer;

// lots of different sized specular spheres and simple fog
Scene Scene::createTestScene8()
{
	Scene scene;

	scene.raytracer.backgroundColor = Color(1.0, 1.0, 1.0);
	scene.raytracer.maxRayIterations = 4;
	scene.raytracer.multiSamples = 0;
	scene.raytracer.cameraSamples = 0;

	scene.rootBVH.enabled = true;

	scene.simpleFog.enabled = true;
	scene.simpleFog.color = Color(1.0, 1.0, 1.0);
	scene.simpleFog.distance = 200.0;
	scene.simpleFog.steepness = 2.0;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 8.0, -2.0);
	scene.camera.orientation = EulerAngle(-20.0, 180.0, 0.0);
	scene.camera.apertureSize = 0.2;
	scene.camera.focalDistance = 30.0;
	
	// GROUND //

	ColorTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color = Color(1.0, 1.0, 1.0);
	groundTexture.intensity = 0.5;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.colorTextureId = groundTexture.id;
	groundMaterial.rayReflectance = 0.3;

	Plane groundPlane;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();

	scene.textures.colorTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// SPHERES //

	std::mt19937 gen(8972364);
	std::uniform_real_distribution<double> radiusDist(1.0, 2.0);
	std::uniform_real_distribution<double> pxDist(2.0, 3.0);
	std::uniform_real_distribution<double> pyDist(-1.0, 1.0);
	std::uniform_real_distribution<double> specularityDist(0.0, 1.0);
	std::uniform_real_distribution<double> shininessDist(4.0, 64.0);

	int index = 1;
	double px;
	double py = -250.0 + radiusDist(gen);

	for (int y = 0; y < 100; ++y)
	{
		px = -200.0 + radiusDist(gen);

		for (int x = 0; x < 100; ++x)
		{
			++index;

			ColorTexture sphereTexture;
			sphereTexture.id = index;
			sphereTexture.color = Color::random(gen);
			sphereTexture.intensity = 0.5;

			Material sphereMaterial;
			sphereMaterial.id = index;
			sphereMaterial.colorTextureId = sphereTexture.id;
			sphereMaterial.rayReflectance = 0.05;
			sphereMaterial.diffuseReflectance = sphereTexture.color;
			sphereMaterial.specularReflectance = Color(1.0, 1.0, 1.0);
			sphereMaterial.shininess = 256.0;

			Sphere sphere;
			sphere.radius = radiusDist(gen);
			sphere.position = Vector3(px, sphere.radius, py + pyDist(gen));
			sphere.materialId = sphereMaterial.id;

			scene.textures.colorTextures.push_back(sphereTexture);
			scene.materials.push_back(sphereMaterial);
			scene.primitives.spheres.push_back(sphere);

			px += sphere.radius + pxDist(gen);
		}

		py += 5.0;
	}

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;
	
	DirectionalLight directionalLight1;
	directionalLight1.color = Color(1.0, 1.0, 1.0);
	directionalLight1.intensity = 1.0;
	directionalLight1.direction = EulerAngle(-30.0, 30.0, 0.0).getDirection();

	scene.lights.directionalLights.push_back(directionalLight1);

	return scene;
}
