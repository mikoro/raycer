// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// reflection and refraction with boxes
Scene Scene::createTestScene5()
{
	Scene scene;

	scene.raytracer.backgroundColor = Color(0.8, 0.8, 0.8);
	scene.raytracer.maxRayIterations = 8;
	scene.raytracer.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(9.49, 4.44, 3.94);
	scene.camera.orientation = EulerAngle(-20.0, 35.0, 0.0);

	// BOX 1 //

	ColorTexture box1Texture;
	box1Texture.id = 3;
	box1Texture.color = Color(1.0, 1.0, 1.0);
	box1Texture.intensity = 1.0;

	Material box1Material;
	box1Material.id = 3;
	box1Material.ambientMapTextureId = box1Texture.id;
	box1Material.diffuseMapTextureId = box1Texture.id;
	box1Material.ambientReflectance = Color(0.5, 0.0, 0.0);
	box1Material.diffuseReflectance = Color(0.5, 0.0, 0.0);
	box1Material.rayTransmittance = 0.75;
	box1Material.refractiveIndex = 1.0;
	box1Material.enableAttenuation = true;
	box1Material.attenuationFactor = 1.0;
	box1Material.attenuationColor = Color(0.1, 0.0, 0.0);
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

	ModelLoaderInfo modelInfo1;
	modelInfo1.modelFilePath = "data/meshes/cube1.obj";
	modelInfo1.defaultMaterialId = box1Material.id;
	modelInfo1.addAllInstance = true;

	modelInfo1.translate = Vector3(4.0, 1.0, 0.0);
	modelInfo1.idStartOffset = 1000;
	modelInfo1.allGroupId = 100;
	scene.models.push_back(modelInfo1);
	modelInfo1.idStartOffset = 2000;
	modelInfo1.allGroupId = 101;
	modelInfo1.translate = Vector3(8.0, 1.0, 0.0);
	scene.models.push_back(modelInfo1);

	// BOX 2 //

	ColorTexture box2Texture;
	box2Texture.id = 4;
	box2Texture.color = Color(1.0, 1.0, 1.0);
	box2Texture.intensity = 1.0;

	Material box2Material;
	box2Material.id = 4;
	box2Material.ambientMapTextureId = box2Texture.id;
	box2Material.diffuseMapTextureId = box2Texture.id;
	box2Material.ambientReflectance = Color(0.1, 0.1, 0.3);
	box2Material.diffuseReflectance = Color(0.0, 0.0, 0.0);
	box2Material.rayReflectance = 0.75;
	box2Material.nonShadowing = true;

	Box box2;
	box2.id = 1;
	box2.materialId = box2Material.id;
	box2.invisible = true;
	box2.extent = Vector3(20.0, 10.0, 1.0);
	box2.position = Vector3(0.0, 0.0, 0.0);

	scene.textures.colorTextures.push_back(box2Texture);
	scene.materials.push_back(box2Material);
	scene.primitives.boxes.push_back(box2);

	// INSTANCE 1 //

	Instance instance1;
	instance1.primitiveId = box2.id;

	instance1.translate = Vector3(0.0, 0.0, -5.0);
	instance1.rotate = EulerAngle(0.0, 0.0, 0.0);
	scene.primitives.instances.push_back(instance1);

	instance1.translate = Vector3(0.0, 0.0, 5.0);
	instance1.rotate = EulerAngle(0.0, 0.0, 0.0);
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
