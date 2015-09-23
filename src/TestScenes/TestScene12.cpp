// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// ambient occlusion with four white spheres and a monkey
Scene Scene::createTestScene12()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(2.3, 5.0, 6.0);
	scene.camera.orientation = EulerAngle(-35.0, 0.0, 0.0);

	// GROUND //

	ColorTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color = Color(1.0, 1.0, 1.0);
	groundTexture.intensity = 0.5;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.ambientMapTextureId = groundTexture.id;
	groundMaterial.diffuseMapTextureId = groundTexture.id;
	groundMaterial.ambientReflectance = Color(1.0, 1.0, 1.0);

	Plane groundPlane;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.001, 1.0, 0.001).normalized();
	
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
	sphere1Material.ambientMapTextureId = sphere1Texture.id;
	sphere1Material.diffuseMapTextureId = sphere1Texture.id;

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

	// MESH 1 //

	ColorTexture mesh1Texture;
	mesh1Texture.id = 3;
	mesh1Texture.color = Color(1.0, 1.0, 1.0);
	mesh1Texture.intensity = 1.0;

	Material mesh1Material;
	mesh1Material.id = 3;
	mesh1Material.ambientMapTextureId = mesh1Texture.id;
	mesh1Material.diffuseMapTextureId = mesh1Texture.id;

	ModelLoaderInfo modelInfo1;
	modelInfo1.modelFilePath = "data/meshes/monkey3.obj";
	modelInfo1.defaultMaterialId = mesh1Material.id;
	modelInfo1.addAllInstance = true;
	modelInfo1.allGroupId = 1;
	modelInfo1.scale = Vector3(5.0, 5.0, 5.0);
	modelInfo1.rotate = EulerAngle(-45.0, 0.0, 0.0);
	modelInfo1.translate = Vector3(5.0, 0.0, 1.0);

	scene.textures.colorTextures.push_back(mesh1Texture);
	scene.materials.push_back(mesh1Material);
	scene.models.push_back(modelInfo1);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.4;
	scene.lights.ambientLight.enableOcclusion = true;
	scene.lights.ambientLight.maxDistance = 10.0;
	scene.lights.ambientLight.samples = 3;

	return scene;
}
