// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>

#include "Raytracing/Scene.h"

using namespace Raycer;

// reflection and refraction with boxes
Scene Scene::createTestScene5()
{
	Scene scene;

	scene.rootBVH.enabled = true;
	scene.raytracing.backgroundColor = Color::WHITE;
	scene.raytracing.maxIterations = 16;

	// CAMERA //

	scene.camera.position = Vector3(13.0, 5.0, 5.3);
	scene.camera.orientation = EulerAngle(-18.0, 50.0, 0.0);

	// BOX 1 //

	ColorTexture box1Texture;
	box1Texture.id = 3;
	box1Texture.color = Color(1.0, 0.8, 0.8);
	box1Texture.intensity = 1.0;

	Material box1Material;
	box1Material.id = 3;
	box1Material.colorTextureId = box1Texture.id;
	box1Material.ambientReflectance = Color(0.0, 0.0, 0.0);
	box1Material.diffuseReflectance = Color(0.1, 0.1, 0.1);
	box1Material.rayTransmittance = 1.0;
	box1Material.refractiveIndex = 1.0;
	box1Material.nonShadowing = true;

	Box box1;
	box1.extent = Vector3(2.0, 2.0, 2.0);
	box1.materialId = box1Material.id;

	scene.textures.colorTextures.push_back(box1Texture);
	scene.materials.push_back(box1Material);

	box1.position = Vector3(-4.0, 1.0, 0.0);
	scene.primitives.boxes.push_back(box1);
	box1.position = Vector3(-8.0, 1.0, 0.0);
	scene.primitives.boxes.push_back(box1);

	// MESH 1 //

	Mesh mesh1;
	mesh1.materialId = box1Material.id;
	mesh1.meshFilePath = "data/meshes/cube1.obj";

	mesh1.position = Vector3(4.0, 1.0, 0.0);
	scene.primitives.meshes.push_back(mesh1);
	mesh1.position = Vector3(8.0, 1.0, 0.0);
	scene.primitives.meshes.push_back(mesh1);

	// BOX 2 //

	ColorTexture box2Texture;
	box2Texture.id = 4;
	box2Texture.color = Color(0.8, 0.8, 1.0);
	box2Texture.intensity = 1.0;

	Material box2Material;
	box2Material.id = 4;
	box2Material.colorTextureId = box2Texture.id;
	box2Material.ambientReflectance = Color(0.0, 0.0, 0.0);
	box2Material.diffuseReflectance = Color(0.1, 0.1, 0.1);
	box2Material.rayReflectance = 1.0;
	box2Material.nonShadowing = true;

	Box box2;
	box2.extent = Vector3(20.0, 10.0, 1.0);
	box2.materialId = box2Material.id;
	box2.position = Vector3(0.0, 100.0, 0.0);

	scene.textures.colorTextures.push_back(box2Texture);
	scene.materials.push_back(box2Material);
	scene.primitives.boxes.push_back(box2);

	// INSTANCE 1 //

	ColorTexture instance1Texture;
	instance1Texture.id = 5;
	instance1Texture.color = Color(0.8, 0.8, 1.0);
	instance1Texture.intensity = 0.8;

	Material instance1Material;
	instance1Material.id = 5;
	instance1Material.colorTextureId = instance1Texture.id;
	instance1Material.ambientReflectance = Color(0.0, 0.0, 0.0);
	instance1Material.diffuseReflectance = Color(0.1, 0.1, 0.1);
	instance1Material.rayReflectance = 1.0;
	instance1Material.nonShadowing = true;

	Instance instance1;
	instance1.materialId = instance1Material.id;
	instance1.primitive = &scene.primitives.boxes.back();

	scene.textures.colorTextures.push_back(instance1Texture);
	scene.materials.push_back(instance1Material);

	instance1.translate = Vector3(0.0, -100.0, -5.0);
	instance1.rotate = EulerAngle(-1.0, 0.0, 0.0);
	scene.primitives.instances.push_back(instance1);

	instance1.translate = Vector3(0.0, -100.0, 5.0);
	instance1.rotate = EulerAngle(1.0, 0.0, 0.0);
	scene.primitives.instances.push_back(instance1);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	DirectionalLight directionalLight1;
	directionalLight1.color = Color(1.0, 1.0, 1.0);
	directionalLight1.intensity = 1.0;
	directionalLight1.direction = EulerAngle(-10.0, 30.0, 0.0).getDirection();

	scene.lights.directionalLights.push_back(directionalLight1);

	return scene;
}
