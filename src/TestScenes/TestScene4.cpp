// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// cornell box
Scene Scene::createTestScene4()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 1.0, 3.5);
	scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);

	// OBJ SCENES

	ObjScene objScene;
	objScene.filePath = "data/meshes/cornellbox.obj";
	objScene.scale = Vector3(1.0, 1.0, 1.0);
	scene.objScenes.push_back(objScene);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.01;

	PointLight pointLight1;
	pointLight1.color = Color(1.0, 1.0, 1.0);
	pointLight1.intensity = 1.0;
	pointLight1.position = Vector3(0.0, 1.9, 0.0);
	pointLight1.distance = 3.0;
	pointLight1.attenuation = 1.0;

	scene.lights.pointLights.push_back(pointLight1);

	return scene;
}
