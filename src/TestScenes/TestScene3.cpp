// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "TestScenes/TestScene.h"
#include "Raytracing/Scene.h"

using namespace Raycer;

// ambient occlusion with four white spheres
Scene TestScene::createTestScene3()
{
	Scene scene;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 4.0, 3.5);
	scene.camera.orientation = EulerAngle(0.0, -45.0, 0.0);

	// GROUND //

	ColorTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color = Color(1.0, 1.0, 1.0);
	groundTexture.intensity = 0.5;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.textureId = groundTexture.id;

	Plane groundPlane;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();
	
	scene.textures.colorTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// SPHERE 1 //

	ColorTexture sphere1Texture;
	sphere1Texture.id = 2;
	sphere1Texture.color = Color(1.0, 1.0, 1.0);
	sphere1Texture.intensity = 1.0;

	Material sphere1Material;
	sphere1Material.id = 2;
	sphere1Material.textureId = sphere1Texture.id;

	Sphere sphere1;
	sphere1.materialId = sphere1Material.id;
	sphere1.radius = 1.0;

	scene.textures.colorTextures.push_back(sphere1Texture);
	scene.materials.push_back(sphere1Material);

	sphere1.position = Vector3(-1.0, 1.0, 1.0);
	scene.primitives.spheres.push_back(sphere1);
	sphere1.position = Vector3(1.0, 1.0, 1.0);
	scene.primitives.spheres.push_back(sphere1);
	sphere1.position = Vector3(-1.0, 1.0, -1.0);
	scene.primitives.spheres.push_back(sphere1);
	sphere1.position = Vector3(1.0, 1.0, -1.0);
	scene.primitives.spheres.push_back(sphere1);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.4;
	scene.lights.ambientLight.ambientOcclusion = true;
	scene.lights.ambientLight.samples = 3;

	return scene;
}
