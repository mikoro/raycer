// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

Scene Scene::createTestScene1()
{
	Scene scene;

	scene.tracer.maxIterations = 4;

	scene.multisampler.type = MultisampleType::CORRELATED_MULTI_JITTER;
	scene.multisampler.multisamples = 6;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 3.0, 6.0);
	scene.camera.orientation = EulerAngle(0.0, -20.0, 0.0);
	scene.camera.fov = 75.0;

	// FOG //

	scene.fog.enabled = false;
	scene.fog.color = Color(0.7, 0.7, 0.7);
	scene.fog.distance = 100.0;
	scene.fog.steepness = 4.0;
	scene.fog.heightDispersion = true;
	scene.fog.height = 20.0;
	scene.fog.heightSteepness = 0.5;

	// ATMOSPHERE //

	AtmosphereTexture atmosphereTexture;
	atmosphereTexture.id = 0;
	atmosphereTexture.intensity = 1.0;
	atmosphereTexture.sunColor = Color(192, 191, 173);
	atmosphereTexture.sunOrientation = EulerAngle(45.0, -45.0, 0.0);

	Material atmosphereMaterial;
	atmosphereMaterial.id = 0;
	atmosphereMaterial.textureId = atmosphereTexture.id;
	atmosphereMaterial.skipLighting = true;

	Sphere atmosphereSphere;
	atmosphereSphere.materialId = atmosphereMaterial.id;
	atmosphereSphere.texcoordScale = Vector2(1.0, 1.0);
	atmosphereSphere.position = Vector3(0.0, 0.0, 0.0);
	atmosphereSphere.radius = 100.0;
	atmosphereSphere.nonShadowing = true;

	//scene.textures.atmosphereTextures.push_back(atmosphereTexture);
	//scene.materials.push_back(atmosphereMaterial);
	//scene.primitives.spheres.push_back(atmosphereSphere);

	// GROUND //

	/*
	ImageTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.imageFilePath = "data/images/dirt1.jpg";
	groundTexture.intensity = 0.5;
	*/

	/*
	ColorTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color = Color(1.0, 1.0, 1.0);
	groundTexture.intensity = 0.3;
	*/

	CheckerTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color1 = Color(0.1, 0.1, 0.1);
	groundTexture.color2 = Color(0.3, 0.3, 0.3);
	groundTexture.intensity = 1.0;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.textureId = groundTexture.id;
	groundMaterial.ambientness = 1.0;
	groundMaterial.diffuseness = 1.0;
	groundMaterial.specularity = 0.2;
	groundMaterial.shininess = 2.0;
	groundMaterial.reflectance = 0.0;
	groundMaterial.transmittance = 0.0;
	groundMaterial.refractiveIndex = 1.0;

	Plane groundPlane;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();
	groundPlane.texcoordScale = Vector2(5.0, 5.0);

	scene.textures.checkerTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// SPHERE 1 //

	VoronoiTexture sphere1Texture;
	sphere1Texture.id = 2;
	sphere1Texture.intensity = 0.5;
	sphere1Texture.seed = 678675;
	sphere1Texture.distanceType = CellNoiseDistanceType::CHEBYSHEV;
	sphere1Texture.density = 1;
	sphere1Texture.scale = Vector3(5.0, 5.0, 5.0);
	sphere1Texture.useRandomColors = true;
	sphere1Texture.randomColorCount = 1000;

	Material sphere1Material;
	sphere1Material.id = 2;
	sphere1Material.textureId = sphere1Texture.id;
	sphere1Material.ambientness = 1.0;
	sphere1Material.diffuseness = 1.0;
	sphere1Material.specularity = 1.0;
	sphere1Material.shininess = 32.0;
	sphere1Material.reflectance = 0.0;
	sphere1Material.transmittance = 0.0;
	sphere1Material.refractiveIndex = 1.0;

	Sphere sphere1;
	sphere1.materialId = sphere1Material.id;
	sphere1.texcoordScale = Vector2(1.0, 1.0);
	sphere1.position = Vector3(0.0, 1.0, 0.0);
	sphere1.radius = 1.0;

	//scene.textures.voronoiTextures.push_back(sphere1Texture);
	//scene.materials.push_back(sphere1Material);
	//scene.primitives.spheres.push_back(sphere1);

	// SPHERE 2 //

	ColorTexture sphere2Texture;
	sphere2Texture.id = 3;
	sphere2Texture.color = Color(1.0, 1.0, 1.0);
	sphere2Texture.intensity = 1.0;

	Material sphere2Material;
	sphere2Material.id = 3;
	sphere2Material.textureId = sphere2Texture.id;
	sphere2Material.ambientness = 0.0;
	sphere2Material.diffuseness = 0.0;
	sphere2Material.specularity = 0.5;
	sphere2Material.shininess = 128.0;
	sphere2Material.reflectance = 1.0;
	sphere2Material.transmittance = 1.0;
	sphere2Material.refractiveIndex = 1.5;
	sphere2Material.fresnel = true;
	sphere2Material.attenuate = true;
	sphere2Material.attenuation = 0.6;
	sphere2Material.attenuationColor = Color(0.0, 0.0, 0.0);

	Sphere sphere2;
	sphere2.materialId = sphere2Material.id;
	sphere2.texcoordScale = Vector2(1.0, 1.0);
	sphere2.position = Vector3(-3.0, 1.0, 0.0);
	sphere2.radius = 1.0;

	//scene.textures.colorTextures.push_back(sphere2Texture);
	//scene.materials.push_back(sphere2Material);
	//scene.primitives.spheres.push_back(sphere2);

	// MESH 1 //

	ColorTexture mesh1Texture;
	mesh1Texture.id = 4;
	mesh1Texture.color = Color(0.8, 0.8, 0.8);
	mesh1Texture.intensity = 1.0;

	Material mesh1Material;
	mesh1Material.id = 4;
	mesh1Material.textureId = mesh1Texture.id;
	mesh1Material.ambientness = 1.0;
	mesh1Material.diffuseness = 1.0;
	mesh1Material.specularity = 0.0;
	mesh1Material.shininess = 2.0;
	mesh1Material.reflectance = 0.0;
	mesh1Material.transmittance = 0.0;
	mesh1Material.refractiveIndex = 1.5;
	mesh1Material.fresnel = false;
	mesh1Material.attenuate = false;
	mesh1Material.attenuation = 0.1;
	mesh1Material.attenuationColor = Color(0.0, 0.0, 0.0);

	Mesh mesh1;
	mesh1.materialId = mesh1Material.id;
	mesh1.meshFilePath = "data/meshes/monkey2.obj";
	mesh1.texcoordScale = Vector2(1.0, 1.0);
	mesh1.position = Vector3(0.0, 2.0, 0.0);
	mesh1.scale = Vector3(1.0, 1.0, 1.0);
	mesh1.orientation = EulerAngle(0.0, 0.0, 0.0);

	//scene.textures.colorTextures.push_back(mesh1Texture);
	//scene.materials.push_back(mesh1Material);
	//scene.primitives.meshes.push_back(mesh1);

	// LIGHTS //

	AmbientLight ambientLight1;
	ambientLight1.color = Color(1.0, 1.0, 1.0);
	ambientLight1.intensity = 0.01;

	scene.lights.ambientLights.push_back(ambientLight1);

	DirectionalLight directionalLight1;
	directionalLight1.color = Color(192, 191, 173);
	directionalLight1.direction = EulerAngle(45.0, -45.0, 0.0).getDirectionVector();
	directionalLight1.intensity = 1.5;

	scene.lights.directionalLights.push_back(directionalLight1);

	PointLight pointLight1;
	pointLight1.color = Color(192, 191, 173);;
	pointLight1.position = Vector3(0.0, 10.0, 0.0);
	pointLight1.distance = 20.0;
	pointLight1.attenuation = 1.0;
	pointLight1.intensity = 1.5;

	//scene.lights.pointLights.push_back(pointLight1);

	SpotLight spotLight1;
	spotLight1.color = Color(192, 191, 173);;
	spotLight1.position = Vector3(20.0, 10.0, 0.0);
	spotLight1.direction = EulerAngle(90.0, -20.0, 0.0).getDirectionVector();
	spotLight1.distance = 1000.0;
	spotLight1.distanceAttenuation = 1.0;
	spotLight1.sideAttenuation = 1.0;
	spotLight1.angle = 15.0;
	spotLight1.intensity = 1.5;

	//scene.lights.spotLights.push_back(spotLight1);

	return scene;
}

Scene Scene::createTestScene2()
{
	Scene scene;
	return scene;
}
