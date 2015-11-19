// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// sponza atrium scene
Scene Scene::createTestScene18()
{
	Scene scene;

	scene.rootBVH.enabled = true;

	// CAMERA //

	scene.camera.position = Vector3(8.92, 0.68, -2.02);
	scene.camera.orientation = EulerAngle(6.66, 111.11, 0.0);
	scene.camera.focalDistance = 10.0;
	scene.camera.apertureSize = 0.005;

	// MODEL //

	ModelLoaderInfo modelInfo(ModelLoaderPreset::TRIANGLES);
	modelInfo.modelFilePath = "data/meshes/sponza/sponza.obj";
	modelInfo.scale = Vector3(0.01, 0.01, 0.01);
	modelInfo.baseMaterial.normalInterpolation = true;
	scene.models.push_back(modelInfo);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;
	scene.lights.ambientLight.enableAmbientOcclusion = false;
	scene.lights.ambientLight.ambientOcclusionMaxSampleDistance = 0.5;
	scene.lights.ambientLight.ambientOcclusionSampleCountSqrt = 8;

	PointLight pointLight;
	pointLight.color = Color(1.0, 1.0, 1.0);
	pointLight.intensity = 1.0;
	pointLight.position = Vector3(0.0, 8.0, 0.0);
	pointLight.maxDistance = 20.0;
	pointLight.attenuation = 1.0;

	scene.lights.pointLights.push_back(pointLight);

	return scene;
}
