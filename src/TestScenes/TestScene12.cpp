// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// relativistic effects
Scene Scene::createTestScene12()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 0.0, 0.0);
	scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);

	return scene;
}
