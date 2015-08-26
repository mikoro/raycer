// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>

#include "Raytracing/Scene.h"

using namespace Raycer;

// soft shadows with boxes
Scene Scene::createTestScene16()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(0.44, 7.05, 7.33);
	scene.camera.orientation = EulerAngle(-26.56, 22.22, 0.0);

	// GROUND //

	ColorTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color = Color(1.0, 1.0, 1.0);
	groundTexture.intensity = 0.3;

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

	// BOX 1 //

	ColorTexture box1Texture;
	box1Texture.id = 3;
	box1Texture.color = Color(0.2, 0.2, 1.0);
	box1Texture.intensity = 0.5;

	Material box1Material;
	box1Material.id = 3;
	box1Material.colorTextureId = box1Texture.id;

	Box box1;
	box1.materialId = box1Material.id;

	scene.textures.colorTextures.push_back(box1Texture);
	scene.materials.push_back(box1Material);

	box1.position = Vector3(0.0, 3.0, 0.0);
	box1.extent = Vector3(2.0, 6.0, 2.0);
	scene.primitives.boxes.push_back(box1);
	box1.position = Vector3(-5.0, 0.5, -2.5);
	box1.extent = Vector3(2.0, 1.0, 5.0);
	scene.primitives.boxes.push_back(box1);
	
	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	PointLight pointLight1;
	pointLight1.color = Color(1.0, 1.0, 1.0);
	pointLight1.intensity = 1.5;
	pointLight1.position = Vector3(10.0, 10.0, 10.0);
	pointLight1.distance = 100.0;
	pointLight1.attenuation = 1.0;
	pointLight1.softShadows = true;
	pointLight1.samples = 3;
	pointLight1.radius = 0.5;

	scene.lights.pointLights.push_back(pointLight1);

	return scene;
}
