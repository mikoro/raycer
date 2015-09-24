// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// sponza1 scene
Scene Scene::createTestScene14()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	scene.rootBVH.enabled = true;
	scene.rootBVH.buildInfo.maxLeafSize = 10;

	// CAMERA //

	scene.camera.position = Vector3(15.0, 2.0, 0.0);
	scene.camera.orientation = EulerAngle(1.0, 90.0, 0.0);

	// MODEL //

	ModelLoaderInfo modelInfo;
	modelInfo.modelFilePath = "data/meshes/sponza1/sponza.obj";
	modelInfo.invisibleTriangles = false;
	modelInfo.enableCombinedGroup = false;
	modelInfo.enableCombinedGroupInstance = false;
	scene.models.push_back(modelInfo);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.01;

	PointLight pointLight1;
	pointLight1.color = Color(1.0, 1.0, 1.0);
	pointLight1.intensity = 1.0;
	pointLight1.position = Vector3(6.5, 8.0, 0.0);
	pointLight1.distance = 10.0;
	pointLight1.attenuation = 1.0;

	PointLight pointLight2;
	pointLight2.color = Color(1.0, 1.0, 1.0);
	pointLight2.intensity = 1.0;
	pointLight2.position = Vector3(-6.5, 8.0, 0.0);
	pointLight2.distance = 10.0;
	pointLight2.attenuation = 1.0;

	scene.lights.pointLights.push_back(pointLight1);
	scene.lights.pointLights.push_back(pointLight2);

	return scene;
}
