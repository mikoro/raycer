// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>

#include "Raytracing/Scene.h"

using namespace Raycer;

// bump mapping with a sphere and plane
Scene Scene::createTestScene13()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 5.0, 5.0);
	scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);

	// GROUND //

	ColorTexture groundTexture1;
	groundTexture1.id = 1;
	groundTexture1.color = Color(1.0, 1.0, 1.0);
	groundTexture1.intensity = 0.5;

	ImageTexture groundTexture2;
	groundTexture2.id = 2;
	groundTexture2.imageFilePath = "data/images/bumpmap.png";
	groundTexture2.applyGamma = false;
	groundTexture2.isBumpMap = true;
	groundTexture2.intensity = 10.0;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.colorTextureId = groundTexture1.id;
	groundMaterial.normalMapTextureId = groundTexture2.id;
	groundMaterial.texcoordScale = Vector2(10.0, 10.0);

	Plane groundPlane;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.01, 1.0, 0.01).normalized();
	
	scene.textures.colorTextures.push_back(groundTexture1);
	scene.textures.imageTextures.push_back(groundTexture2);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// SPHERE 1 //

	ColorTexture sphere1Texture1;
	sphere1Texture1.id = 3;
	sphere1Texture1.color = Color(1.0, 0.1, 0.1);
	sphere1Texture1.intensity = 0.5;

	PerlinNoiseTexture sphere1Texture2;
	sphere1Texture2.id = 4;
	sphere1Texture2.seed = 2345654;
	sphere1Texture2.intensity = 0.05;
	sphere1Texture2.scale = Vector3(10.0, 10.0, 10.0);
	sphere1Texture2.isFbm = true;

	Material sphere1Material;
	sphere1Material.id = 2;
	sphere1Material.colorTextureId = sphere1Texture1.id;
	sphere1Material.normalMapTextureId = sphere1Texture2.id;
	sphere1Material.diffuseReflectance = Color(0.5, 0.5, 0.5);
	sphere1Material.specularReflectance = Color(1.0, 1.0, 1.0);
	sphere1Material.shininess = 64.0;
	sphere1Material.nonShadowing = true;

	Sphere sphere1;
	sphere1.materialId = sphere1Material.id;
	sphere1.position = Vector3(0.0, 1.0, 0.0);
	sphere1.radius = 1.0;

	scene.textures.colorTextures.push_back(sphere1Texture1);
	scene.textures.perlinNoiseTextures.push_back(sphere1Texture2);
	scene.materials.push_back(sphere1Material);
	scene.primitives.spheres.push_back(sphere1);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.01;

	PointLight pointLight1;
	pointLight1.color = Color(1.0, 1.0, 1.0);
	pointLight1.intensity = 0.8;
	pointLight1.distance = 20.0;
	pointLight1.attenuation = 1.0;

	pointLight1.position = Vector3(5.0, 5.0, 5.0);
	scene.lights.pointLights.push_back(pointLight1);
	pointLight1.position = Vector3(-5.0, 5.0, 5.0);
	scene.lights.pointLights.push_back(pointLight1);

	return scene;
}
