// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "TestScenes/TestScene.h"
#include "Raytracing/Scene.h"

using namespace Raycer;

// depth of field with a cube spiral
Scene TestScene::createTestScene5()
{
	Scene scene;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 0.0, 7.0);
	scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);
	scene.camera.depthOfField = true;
	scene.camera.apertureSize = 0.5;
	scene.camera.focalLenght = 30.0;
	scene.camera.samples = 3;

	// BOX 1 //

	ColorTexture box1Texture;
	box1Texture.id = 2;
	box1Texture.color = Color(0.0, 1.0, 1.0);
	box1Texture.intensity = 0.5;

	Material box1Material;
	box1Material.id = 2;
	box1Material.textureId = box1Texture.id;

	scene.textures.colorTextures.push_back(box1Texture);
	scene.materials.push_back(box1Material);

	double angle = -M_PI / 2.0;
	double radius = 8.0;
	
	for (int i = 0; i < 50; ++i)
	{
		Box box1 = Box::create(Vector3(cos(angle) * radius, sin(angle) * radius, i * -2.0), Vector3(1.0, 1.0, 1.0));
		box1.materialId = box1Material.id;
		scene.primitives.boxes.push_back(box1);
		angle += 0.5;
	}

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;
	
	DirectionalLight directionalLight1;
	directionalLight1.color = Color(1.0, 1.0, 1.0);
	directionalLight1.intensity = 1.0;
	directionalLight1.direction = EulerAngle(-60.0, -10.0, 0.0).getDirectionVector();

	scene.lights.directionalLights.push_back(directionalLight1);

	return scene;
}
