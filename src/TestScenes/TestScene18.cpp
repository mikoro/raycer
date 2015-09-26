// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// sponza atrium scene
Scene Scene::createTestScene18()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;
	scene.raytracer.cameraSamples = 0;

	scene.rootBVH.enabled = true;

	// CAMERA //

	scene.camera.position = Vector3(8.92, 0.68, -2.02);
	scene.camera.orientation = EulerAngle(6.66, 111.11, 0.0);
	scene.camera.focalDistance = 10.0;
	scene.camera.apertureSize = 0.005;

	// MODEL //

	ModelLoaderInfo modelInfo(ModelLoaderInfoType::TRIANGLES);
	modelInfo.modelFilePath = "data/meshes/sponza/sponza.obj";
	modelInfo.scale = Vector3(0.01, 0.01, 0.01);
	scene.models.push_back(modelInfo);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.05;

	PointLight pointLight;
	pointLight.color = Color(1.0, 1.0, 1.0);
	pointLight.intensity = 1.0;
	pointLight.position = Vector3(0.0, 8.0, 0.0);
	pointLight.distance = 20.0;
	pointLight.attenuation = 2.0;

	scene.lights.pointLights.push_back(pointLight);

	return scene;
}
