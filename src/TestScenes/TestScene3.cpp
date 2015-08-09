// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "TestScenes/TestScene.h"
#include "Raytracing/Scene.h"

using namespace Raycer;

// spherical texture map
Scene TestScene::createTestScene3()
{
	Scene scene;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 0.0, 0.0);
	scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);

	// SPHERE 1 //

	ImageTexture sphere1Texture;
	sphere1Texture.id = 1;
	sphere1Texture.intensity = 1.0;
	sphere1Texture.imageFilePath = "data/images/rooftop.hdr";

	Material sphere1Material;
	sphere1Material.id = 1;
	sphere1Material.textureId = sphere1Texture.id;
	sphere1Material.skipLighting = true;
	sphere1Material.nonShadowing = true;

	Sphere sphere1;
	sphere1.materialId = sphere1Material.id;
	sphere1.position = Vector3(0.0, 0.0, 0.0);
	sphere1.radius = 100.0;
	sphere1.texcoordScale = Vector2(1.0, -1.0);

	scene.textures.imageTextures.push_back(sphere1Texture);
	scene.materials.push_back(sphere1Material);
	scene.primitives.spheres.push_back(sphere1);

	return scene;
}
