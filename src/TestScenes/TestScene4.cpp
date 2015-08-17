// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>

#include "Raytracing/Scene.h"

using namespace Raycer;

// cornell box
Scene Scene::createTestScene4()
{
	Scene scene;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 1.0, 3.5);
	scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);

	// OBJ SCENES

	scene.objSceneFilePaths.push_back("data/meshes/cornellbox.obj");

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
