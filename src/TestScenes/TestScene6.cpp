// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// different textured spheres, a plane and an atmosphere
Scene Scene::createTestScene6()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	scene.rootBVH.enabled = true;

	scene.simpleFog.enabled = true;
	scene.simpleFog.distance = 200.0;
	scene.simpleFog.steepness = 2.0;
	scene.simpleFog.heightDispersion = true;
	scene.simpleFog.height = 50.0;
	scene.simpleFog.heightSteepness = 0.5;
	scene.simpleFog.color = Color(0.8, 0.8, 0.8);

	// CAMERA //

	scene.camera.position = Vector3(11.0, 6.5, -7.5);
	scene.camera.orientation = EulerAngle(-20.0, 90.0, 0.0);

	// GROUND //

	ImageTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.imageFilePath = "data/images/grass.jpg";
	groundTexture.intensity = 0.5;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.ambientMapTextureId = groundTexture.id;
	groundMaterial.diffuseMapTextureId = groundTexture.id;
	groundMaterial.texcoordScale = Vector2(0.1, 0.1);

	Plane groundPlane;
	groundPlane.id = 1;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();

	scene.textures.imageTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// SPHERE 1 //

	AtmosphereTexture sphere1Texture;
	sphere1Texture.id = 2;
	sphere1Texture.intensity = 0.5;
	sphere1Texture.sunOrientation = EulerAngle(-40.0, 40.0, 0.0);
	sphere1Texture.sunSize = 100.0;

	Material sphere1Material;
	sphere1Material.id = 2;
	sphere1Material.ambientMapTextureId = sphere1Texture.id;
	sphere1Material.diffuseMapTextureId = sphere1Texture.id;
	sphere1Material.skipLighting = true;
	sphere1Material.nonShadowing = true;
	sphere1Material.invertNormal = true;

	Sphere sphere1;
	sphere1.id = 2;
	sphere1.materialId = sphere1Material.id;
	sphere1.position = Vector3(0.0, 0.0, 0.0);
	sphere1.radius = 200.0;

	scene.textures.atmosphereTextures.push_back(sphere1Texture);
	scene.materials.push_back(sphere1Material);
	scene.primitives.spheres.push_back(sphere1);

	// SPHERE 2 //

	ColorTexture sphere2Texture;
	sphere2Texture.id = 3;
	sphere2Texture.color = Color(1.0, 0.5, 0.25);
	sphere2Texture.intensity = 0.5;

	Material sphere2Material;
	sphere2Material.id = 3;
	sphere2Material.ambientMapTextureId = sphere2Texture.id;
	sphere2Material.diffuseMapTextureId = sphere2Texture.id;

	Sphere sphere2;
	sphere2.id = 3;
	sphere2.materialId = sphere2Material.id;
	sphere2.position = Vector3(0.0, 1.0, 0.0);
	sphere2.radius = 1.0;

	scene.textures.colorTextures.push_back(sphere2Texture);
	scene.materials.push_back(sphere2Material);
	scene.primitives.spheres.push_back(sphere2);

	// SPHERE 3 //

	CheckerTexture sphere3Texture;
	sphere3Texture.id = 4;
	sphere3Texture.color1 = Color(1.0, 1.0, 1.0);
	sphere3Texture.color2 = Color(0.0, 0.0, 0.0);
	sphere3Texture.intensity = 0.5;

	Material sphere3Material;
	sphere3Material.id = 4;
	sphere3Material.ambientMapTextureId = sphere3Texture.id;
	sphere3Material.diffuseMapTextureId = sphere3Texture.id;
	sphere3Material.texcoordScale = Vector2(10.0, 10.0);

	Sphere sphere3;
	sphere3.id = 4;
	sphere3.materialId = sphere3Material.id;
	sphere3.position = Vector3(0.0, 1.0, -3.0);
	sphere3.radius = 1.0;

	scene.textures.checkerTextures.push_back(sphere3Texture);
	scene.materials.push_back(sphere3Material);
	scene.primitives.spheres.push_back(sphere3);

	// SPHERE 4 //

	ImageTexture sphere4Texture;
	sphere4Texture.id = 5;
	sphere4Texture.imageFilePath = "data/images/poolball1.jpg";
	sphere4Texture.intensity = 0.5;

	Material sphere4Material;
	sphere4Material.id = 5;
	sphere4Material.ambientMapTextureId = sphere4Texture.id;
	sphere4Material.diffuseMapTextureId = sphere4Texture.id;

	Sphere sphere4;
	sphere4.id = 5;
	sphere4.materialId = sphere4Material.id;
	sphere4.position = Vector3(0.0, 1.0, -6.0);
	sphere4.radius = 1.0;

	scene.textures.imageTextures.push_back(sphere4Texture);
	scene.materials.push_back(sphere4Material);
	scene.primitives.spheres.push_back(sphere4);

	// SPHERE 5 //

	MarbleTexture sphere5Texture;
	sphere5Texture.id = 6;
	sphere5Texture.intensity = 0.5;

	Material sphere5Material;
	sphere5Material.id = 6;
	sphere5Material.ambientMapTextureId = sphere5Texture.id;
	sphere5Material.diffuseMapTextureId = sphere5Texture.id;

	Sphere sphere5;
	sphere5.id = 6;
	sphere5.materialId = sphere5Material.id;
	sphere5.position = Vector3(0.0, 1.0, -9.0);
	sphere5.radius = 1.0;

	scene.textures.marbleTextures.push_back(sphere5Texture);
	scene.materials.push_back(sphere5Material);
	scene.primitives.spheres.push_back(sphere5);

	// SPHERE 6 //

	WoodTexture sphere6Texture;
	sphere6Texture.id = 7;
	sphere6Texture.intensity = 0.5;

	Material sphere6Material;
	sphere6Material.id = 7;
	sphere6Material.ambientMapTextureId = sphere6Texture.id;
	sphere6Material.diffuseMapTextureId = sphere6Texture.id;

	Sphere sphere6;
	sphere6.id = 7;
	sphere6.materialId = sphere6Material.id;
	sphere6.position = Vector3(0.0, 1.0, -12.0);
	sphere6.radius = 1.0;

	scene.textures.woodTextures.push_back(sphere6Texture);
	scene.materials.push_back(sphere6Material);
	scene.primitives.spheres.push_back(sphere6);

	// SPHERE 7 //

	FireTexture sphere7Texture;
	sphere7Texture.id = 8;
	sphere7Texture.intensity = 0.5;

	Material sphere7Material;
	sphere7Material.id = 8;
	sphere7Material.ambientMapTextureId = sphere7Texture.id;
	sphere7Material.diffuseMapTextureId = sphere7Texture.id;

	Sphere sphere7;
	sphere7.id = 8;
	sphere7.materialId = sphere7Material.id;
	sphere7.position = Vector3(0.0, 1.0, -15.0);
	sphere7.radius = 1.0;

	scene.textures.fireTextures.push_back(sphere7Texture);
	scene.materials.push_back(sphere7Material);
	scene.primitives.spheres.push_back(sphere7);

	// SPHERE 8 //

	PerlinNoiseTexture sphere8Texture;
	sphere8Texture.id = 9;
	sphere8Texture.intensity = 0.5;

	Material sphere8Material;
	sphere8Material.id = 9;
	sphere8Material.ambientMapTextureId = sphere8Texture.id;
	sphere8Material.diffuseMapTextureId = sphere8Texture.id;

	Sphere sphere8;
	sphere8.id = 9;
	sphere8.materialId = sphere8Material.id;
	sphere8.position = Vector3(-3.0, 1.0, 0.0);
	sphere8.radius = 1.0;

	scene.textures.perlinNoiseTextures.push_back(sphere8Texture);
	scene.materials.push_back(sphere8Material);
	scene.primitives.spheres.push_back(sphere8);

	// SPHERE 9 //

	CellNoiseTexture sphere9Texture;
	sphere9Texture.id = 10;
	sphere9Texture.intensity = 0.5;

	Material sphere9Material;
	sphere9Material.id = 10;
	sphere9Material.ambientMapTextureId = sphere9Texture.id;
	sphere9Material.diffuseMapTextureId = sphere9Texture.id;

	Sphere sphere9;
	sphere9.id = 10;
	sphere9.materialId = sphere9Material.id;
	sphere9.position = Vector3(-3.0, 1.0, -3.0);
	sphere9.radius = 1.0;

	scene.textures.cellNoiseTextures.push_back(sphere9Texture);
	scene.materials.push_back(sphere9Material);
	scene.primitives.spheres.push_back(sphere9);

	// SPHERE 10 //

	CellNoiseTexture sphere10Texture;
	sphere10Texture.id = 11;
	sphere10Texture.intensity = 0.5;
	sphere10Texture.combineType = CellNoiseCombineType::D2;

	Material sphere10Material;
	sphere10Material.id = 11;
	sphere10Material.ambientMapTextureId = sphere10Texture.id;
	sphere10Material.diffuseMapTextureId = sphere10Texture.id;

	Sphere sphere10;
	sphere10.id = 11;
	sphere10.materialId = sphere10Material.id;
	sphere10.position = Vector3(-3.0, 1.0, -6.0);
	sphere10.radius = 1.0;

	scene.textures.cellNoiseTextures.push_back(sphere10Texture);
	scene.materials.push_back(sphere10Material);
	scene.primitives.spheres.push_back(sphere10);

	// SPHERE 10 //

	CellNoiseTexture sphere11Texture;
	sphere11Texture.id = 12;
	sphere11Texture.intensity = 0.5;
	sphere11Texture.combineType = CellNoiseCombineType::D1_MINUS_D2;

	Material sphere11Material;
	sphere11Material.id = 12;
	sphere11Material.ambientMapTextureId = sphere11Texture.id;
	sphere11Material.diffuseMapTextureId = sphere11Texture.id;

	Sphere sphere11;
	sphere11.id = 12;
	sphere11.materialId = sphere11Material.id;
	sphere11.position = Vector3(-3.0, 1.0, -9.0);
	sphere11.radius = 1.0;

	scene.textures.cellNoiseTextures.push_back(sphere11Texture);
	scene.materials.push_back(sphere11Material);
	scene.primitives.spheres.push_back(sphere11);

	// SPHERE 11 //

	VoronoiTexture sphere12Texture;
	sphere12Texture.id = 13;
	sphere12Texture.intensity = 0.5;
	sphere12Texture.scale = Vector3(8.0, 8.0, 8.0);
	sphere12Texture.seed = 2137896;
	sphere12Texture.distanceType = CellNoiseDistanceType::EUCLIDEAN;

	Material sphere12Material;
	sphere12Material.id = 13;
	sphere12Material.ambientMapTextureId = sphere12Texture.id;
	sphere12Material.diffuseMapTextureId = sphere12Texture.id;

	Sphere sphere12;
	sphere12.id = 13;
	sphere12.materialId = sphere12Material.id;
	sphere12.position = Vector3(-3.0, 1.0, -12.0);
	sphere12.radius = 1.0;

	scene.textures.voronoiTextures.push_back(sphere12Texture);
	scene.materials.push_back(sphere12Material);
	scene.primitives.spheres.push_back(sphere12);

	// SPHERE 12 //

	VoronoiTexture sphere13Texture;
	sphere13Texture.id = 14;
	sphere13Texture.intensity = 0.5;
	sphere13Texture.scale = Vector3(8.0, 8.0, 8.0);
	sphere13Texture.seed = 567567;
	sphere13Texture.distanceType = CellNoiseDistanceType::MANHATTAN;

	Material sphere13Material;
	sphere13Material.id = 14;
	sphere13Material.ambientMapTextureId = sphere13Texture.id;
	sphere13Material.diffuseMapTextureId = sphere13Texture.id;

	Sphere sphere13;
	sphere13.id = 14;
	sphere13.materialId = sphere13Material.id;
	sphere13.position = Vector3(-3.0, 1.0, -15.0);
	sphere13.radius = 1.0;

	scene.textures.voronoiTextures.push_back(sphere13Texture);
	scene.materials.push_back(sphere13Material);
	scene.primitives.spheres.push_back(sphere13);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.01;

	DirectionalLight directionalLight;
	directionalLight.color = Color(1.0, 1.0, 1.0);
	directionalLight.intensity = 1.5;
	directionalLight.direction = EulerAngle(-40.0, 40.0, 0.0).getDirection();

	scene.lights.directionalLights.push_back(directionalLight);

	return scene;
}
