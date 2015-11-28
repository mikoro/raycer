// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// cornell box (only triangles)
Scene Scene::createTestScene3()
{
	Scene scene;

	scene.rootBVH.enabled = true;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 1.0, 3.5);
	scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);

	// MODEL //

	ModelLoaderInfo modelInfo(ModelLoaderPreset::TRIANGLES);
	modelInfo.modelFilePath = "data/meshes/cornellbox.obj";
	modelInfo.combinedGroupId = 1;
	modelInfo.combinedGroupInstanceId = 2;
	modelInfo.idStartOffset = 3;

	scene.models.push_back(modelInfo);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.01;

	PointLight pointLight;
	pointLight.color = Color(1.0, 0.71, 0.24);
	pointLight.intensity = 1.0;
	pointLight.position = Vector3(0.0, 1.9, 0.0);
	pointLight.maxDistance = 3.0;
	pointLight.attenuation = 1.0;

	scene.lights.pointLights.push_back(pointLight);

	return scene;
}
