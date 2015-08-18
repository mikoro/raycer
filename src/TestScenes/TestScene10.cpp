// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>

#include "Raytracing/Scene.h"

using namespace Raycer;

// motion blur with spheres
Scene Scene::createTestScene10()
{
	Scene scene;

	scene.rootBVH.enabled = false;

	scene.raytracing.timeSamples = 4;

	scene.fog.enabled = true;
	scene.fog.distance = 20.0;
	scene.fog.steepness = 2.0;
	scene.fog.color = Color(0.0, 0.0, 0.0);

	// CAMERA //

	scene.camera.position = Vector3(0.0, 7.0, 5.0);
	scene.camera.orientation = EulerAngle(-40.0, 0.0, 0.0);
	
	// GROUND //

	ColorTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color = Color(0.0, 1.0, 0.2);
	groundTexture.intensity = 0.25;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.colorTextureId = groundTexture.id;

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
	sphere1Texture.intensity = 0.5;

	Material sphere1Material;
	sphere1Material.id = 2;
	sphere1Material.colorTextureId = sphere1Texture.id;
	sphere1Material.specularReflectance = Color(0.5, 0.5, 0.5);
	sphere1Material.shininess = 16.0;
	sphere1Material.rayReflectance = 0.5;

	Sphere sphere1;
	sphere1.materialId = sphere1Material.id;
	sphere1.position = Vector3(0.0, 1.0, 1.0);
	sphere1.displacement = Vector3(0.0, 0.0, -2.0);
	sphere1.radius = 1.0;

	scene.textures.colorTextures.push_back(sphere1Texture);
	scene.materials.push_back(sphere1Material);
	scene.primitives.spheres.push_back(sphere1);

	// SPHERE 2 //

	ColorTexture sphere2Texture;
	sphere2Texture.id = 3;
	sphere2Texture.color = Color(1.0, 0.5, 0.0);
	sphere2Texture.intensity = 0.5;

	Material sphere2Material;
	sphere2Material.id = 3;
	sphere2Material.colorTextureId = sphere2Texture.id;
	sphere2Material.specularReflectance = Color(0.5, 0.5, 0.5);
	sphere2Material.shininess = 16.0;
	sphere2Material.rayReflectance = 0.5;

	Sphere sphere2;
	sphere2.materialId = sphere2Material.id;
	sphere2.position = Vector3(-1.0, 1.0, -2.0);
	sphere2.displacement = Vector3(-1.0, 0.0, -1.0);
	sphere2.radius = 1.0;

	scene.textures.colorTextures.push_back(sphere2Texture);
	scene.materials.push_back(sphere2Material);
	scene.primitives.spheres.push_back(sphere2);

	// SPHERE 3 //

	ColorTexture sphere3Texture;
	sphere3Texture.id = 4;
	sphere3Texture.color = Color(0.0, 0.5, 1.0);
	sphere3Texture.intensity = 0.5;

	Material sphere3Material;
	sphere3Material.id = 4;
	sphere3Material.colorTextureId = sphere3Texture.id;
	sphere3Material.specularReflectance = Color(0.5, 0.5, 0.5);
	sphere3Material.shininess = 16.0;
	sphere3Material.rayReflectance = 0.5;

	Sphere sphere3;
	sphere3.materialId = sphere3Material.id;
	sphere3.position = Vector3(1.0, 1.0, -2.0);
	sphere3.displacement = Vector3(1.0, 0.0, -1.0);
	sphere3.radius = 1.0;

	scene.textures.colorTextures.push_back(sphere3Texture);
	scene.materials.push_back(sphere3Material);
	scene.primitives.spheres.push_back(sphere3);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.01;

	PointLight pointLight1;
	pointLight1.color = Color(1.0, 1.0, 1.0);
	pointLight1.intensity = 0.5;
	pointLight1.distance = 20.0;
	pointLight1.attenuation = 1.0;

	pointLight1.position = Vector3(5.0, 5.0, 5.0);
	scene.lights.pointLights.push_back(pointLight1);
	pointLight1.position = Vector3(-5.0, 5.0, 5.0);
	scene.lights.pointLights.push_back(pointLight1);
	pointLight1.position = Vector3(5.0, 5.0, -5.0);
	scene.lights.pointLights.push_back(pointLight1);
	pointLight1.position = Vector3(-5.0, 5.0, -5.0);
	scene.lights.pointLights.push_back(pointLight1);

	return scene;
}
