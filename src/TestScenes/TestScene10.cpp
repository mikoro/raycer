// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// motion blur with spheres
Scene Scene::createTestScene10()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;
	scene.raytracer.timeSamples = 3;

	scene.simpleFog.enabled = false;
	scene.simpleFog.distance = 20.0;
	scene.simpleFog.steepness = 2.0;
	scene.simpleFog.color = Color(0.0, 0.0, 0.0);

	// CAMERA //

	scene.camera.position = Vector3(0.0, 6.0, 5.0);
	scene.camera.orientation = EulerAngle(-40.0, 0.0, 0.0);

	// FLOOR //

	ColorTexture floorTexture;
	floorTexture.id = 1;
	floorTexture.color = Color(0.0, 1.0, 0.2);
	floorTexture.intensity = 0.25;

	PerlinNoiseTexture floorNormalTexture;
	floorNormalTexture.id = 2;
	floorNormalTexture.intensity = 0.025;

	Material floorMaterial;
	floorMaterial.id = 1;
	floorMaterial.colorTextureId = floorTexture.id;
	floorMaterial.normalMapTextureId = floorNormalTexture.id;

	Plane floorPlane;
	floorPlane.materialId = floorMaterial.id;
	floorPlane.position = Vector3(0.0, 0.0, 0.0);
	floorPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();

	scene.textures.colorTextures.push_back(floorTexture);
	scene.textures.perlinNoiseTextures.push_back(floorNormalTexture);
	scene.materials.push_back(floorMaterial);
	scene.primitives.planes.push_back(floorPlane);

	// SPHERE 1 //

	ColorTexture sphere1Texture;
	sphere1Texture.id = 3;
	sphere1Texture.color = Color(1.0, 1.0, 1.0);
	sphere1Texture.intensity = 0.5;

	Material sphere1Material;
	sphere1Material.id = 2;
	sphere1Material.colorTextureId = sphere1Texture.id;
	sphere1Material.specularReflectance = Color(0.8, 0.8, 0.8);
	sphere1Material.shininess = 128.0;

	Sphere sphere1;
	sphere1.id = 1;
	sphere1.invisible = true;
	sphere1.materialId = sphere1Material.id;
	sphere1.radius = 1.0;

	Instance sphere1instance;
	sphere1instance.primitiveId = 1;
	sphere1instance.materialId = sphere1Material.id;
	sphere1instance.isTimeVariant = true;
	sphere1instance.translate = Vector3(0.0, 1.0, 0.0);
	sphere1instance.translateInTime = Vector3(0.0, 0.0, 2.0);

	scene.textures.colorTextures.push_back(sphere1Texture);
	scene.materials.push_back(sphere1Material);
	scene.primitives.spheres.push_back(sphere1);
	scene.primitives.instances.push_back(sphere1instance);

	// SPHERE 2 //

	ImageTexture sphere2Texture;
	sphere2Texture.id = 4;
	sphere2Texture.imageFilePath = "data/images/poolball1.jpg";
	sphere2Texture.intensity = 0.5;

	Material sphere2Material;
	sphere2Material.id = 3;
	sphere2Material.colorTextureId = sphere2Texture.id;
	sphere2Material.specularReflectance = Color(0.8, 0.8, 0.8);
	sphere2Material.shininess = 128.0;
	sphere2Material.texcoordScale = Vector2(-1.0, -1.0);

	Sphere sphere2;
	sphere2.id = 2;
	sphere2.invisible = true;
	sphere2.materialId = sphere2Material.id;
	sphere2.radius = 1.0;

	Instance sphere2instance;
	sphere2instance.primitiveId = 2;
	sphere2instance.materialId = sphere2Material.id;
	sphere2instance.isTimeVariant = true;
	sphere2instance.translate = Vector3(-1.0, 1.0, -1.7);
	sphere2instance.rotate = EulerAngle(0.0, -90.0, 0.0);
	sphere2instance.translateInTime = Vector3(-1.0, 0.0, -1.0);
	sphere2instance.rotateInTime = EulerAngle(0.0, 0.0, 0.0);

	scene.textures.imageTextures.push_back(sphere2Texture);
	scene.materials.push_back(sphere2Material);
	scene.primitives.spheres.push_back(sphere2);
	scene.primitives.instances.push_back(sphere2instance);

	// SPHERE 3 //

	ImageTexture sphere3Texture;
	sphere3Texture.id = 5;
	sphere3Texture.imageFilePath = "data/images/poolball2.jpg";
	sphere3Texture.intensity = 0.5;

	Material sphere3Material;
	sphere3Material.id = 4;
	sphere3Material.colorTextureId = sphere3Texture.id;
	sphere3Material.specularReflectance = Color(0.8, 0.8, 0.8);
	sphere3Material.shininess = 128.0;
	sphere3Material.texcoordScale = Vector2(-1.0, -1.0);

	Sphere sphere3;
	sphere3.id = 3;
	sphere3.invisible = true;
	sphere3.materialId = sphere3Material.id;
	sphere3.radius = 1.0;

	Instance sphere3instance;
	sphere3instance.primitiveId = 3;
	sphere3instance.materialId = sphere3Material.id;
	sphere3instance.isTimeVariant = true;
	sphere3instance.translate = Vector3(1.0, 1.0, -1.7);
	sphere3instance.rotate = EulerAngle(0.0, -90.0, 90.0);
	sphere3instance.translateInTime = Vector3(1.0, 0.0, -1.0);
	sphere3instance.rotateInTime = EulerAngle(0.0, 0.0, 0.0);

	scene.textures.imageTextures.push_back(sphere3Texture);
	scene.materials.push_back(sphere3Material);
	scene.primitives.spheres.push_back(sphere3);
	scene.primitives.instances.push_back(sphere3instance);

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
