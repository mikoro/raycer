// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>

#include "Raytracing/Scene.h"

using namespace Raycer;

// simple triangle, sphere and box on a plane with one point light
Scene Scene::createTestScene1()
{
	Scene scene;

	scene.raytracer.multiSamplerType = SamplerType::CMJ;
	scene.raytracer.multiSamplerFilterType = FilterType::CUBIC_BSPLINE;
	scene.raytracer.multiSamples = 0;
	scene.raytracer.toneMapperType = ToneMapperType::REINHARD;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 5.0, 7.0);
	scene.camera.orientation = EulerAngle(-30.0, 0.0, 0.0);

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

	// SPHERE 1 //

	ColorTexture sphere1Texture;
	sphere1Texture.id = 2;
	sphere1Texture.color = Color(0.0, 1.0, 0.0);
	sphere1Texture.intensity = 0.5;

	Material sphere1Material;
	sphere1Material.id = 2;
	sphere1Material.colorTextureId = sphere1Texture.id;

	Sphere sphere1;
	sphere1.materialId = sphere1Material.id;
	sphere1.position = Vector3(0.0, 1.0, 0.0);
	sphere1.radius = 1.0;

	scene.textures.colorTextures.push_back(sphere1Texture);
	scene.materials.push_back(sphere1Material);
	scene.primitives.spheres.push_back(sphere1);

	// BOX 1 //

	ColorTexture box1Texture;
	box1Texture.id = 3;
	box1Texture.color = Color(0.0, 0.0, 1.0);
	box1Texture.intensity = 0.5;

	Material box1Material;
	box1Material.id = 3;
	box1Material.colorTextureId = box1Texture.id;

	Box box1;
	box1.position = Vector3(3.0, 1.0, 0.0);
	box1.extent = Vector3(2.0, 2.0, 2.0);
	box1.materialId = box1Material.id;

	scene.textures.colorTextures.push_back(box1Texture);
	scene.materials.push_back(box1Material);
	scene.primitives.boxes.push_back(box1);

	// TRIANGLE 1 //

	ColorTexture triangle1Texture;
	triangle1Texture.id = 4;
	triangle1Texture.color = Color(1.0, 0.0, 0.0);
	triangle1Texture.intensity = 0.5;

	Material triangle1Material;
	triangle1Material.id = 4;
	triangle1Material.colorTextureId = triangle1Texture.id;

	Triangle triangle1;
	triangle1.materialId = triangle1Material.id;
	triangle1.vertices[0] = Vector3(-4.0, 0.0, 0.0);
	triangle1.vertices[1] = Vector3(-2.0, 0.0, 0.0);
	triangle1.vertices[2] = Vector3(-3.0, 2.0, 0.0);
	triangle1.normals[0] = triangle1.normals[1] = triangle1.normals[2] = Vector3::FORWARD;

	scene.textures.colorTextures.push_back(triangle1Texture);
	scene.materials.push_back(triangle1Material);
	scene.primitives.triangles.push_back(triangle1);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.01;

	PointLight pointLight1;
	pointLight1.color = Color(1.0, 1.0, 1.0);
	pointLight1.intensity = 1.5;
	pointLight1.position = Vector3(5.0, 5.0, 5.0);
	pointLight1.distance = 20.0;
	pointLight1.attenuation = 1.0;

	scene.lights.pointLights.push_back(pointLight1);

	return scene;
}
