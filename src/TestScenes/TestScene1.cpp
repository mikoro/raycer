// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// simple triangle, sphere and box on a plane with one point light
Scene Scene::createTestScene1()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	// CAMERA //

	//scene.camera.position = Vector3(6.5, 6.0, 9.0);
	scene.camera.position = Vector3(0.0, 6.0, 9.0);
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

	// TRIANGLE //

	ColorTexture triangleTexture;
	triangleTexture.id = 2;
	triangleTexture.color = Color(1.0, 0.0, 0.0);
	triangleTexture.intensity = 0.5;

	Material triangleMaterial;
	triangleMaterial.id = 2;
	triangleMaterial.colorTextureId = triangleTexture.id;

	Triangle triangle;
	triangle.id = 2;
	triangle.invisible = true;
	triangle.materialId = triangleMaterial.id;
	triangle.vertices[0] = Vector3(-1.0, 0.0, 0.0);
	triangle.vertices[1] = Vector3(1.0, 0.0, 0.0);
	triangle.vertices[2] = Vector3(0.0, 2.0, 0.0);
	triangle.normals[0] = triangle.normals[1] = triangle.normals[2] = Vector3::FORWARD;

	Instance instance;
	instance.primitiveId = triangle.id;
	instance.materialId = triangle.materialId;
	instance.translate = Vector3(0.0, 0.5, 0.0);
	instance.rotate = EulerAngle(-10.0, 10.0, 10.0);

	scene.textures.colorTextures.push_back(triangleTexture);
	scene.materials.push_back(triangleMaterial);
	scene.primitives.triangles.push_back(triangle);
	//scene.primitives.instances.push_back(instance);

	// SPHERE //

	ColorTexture sphereTexture;
	sphereTexture.id = 3;
	sphereTexture.color = Color(0.0, 1.0, 0.0);
	sphereTexture.intensity = 0.5;

	Material sphereMaterial;
	sphereMaterial.id = 3;
	sphereMaterial.colorTextureId = sphereTexture.id;

	Sphere sphere;
	sphere.id = 3;
	sphere.invisible = true;
	sphere.materialId = sphereMaterial.id;
	sphere.radius = 1.0;

	instance.primitiveId = sphere.id;
	instance.materialId = sphere.materialId;
	instance.translate = Vector3(4.0, 1.5, 0.0);

	scene.textures.colorTextures.push_back(sphereTexture);
	scene.materials.push_back(sphereMaterial);
	scene.primitives.spheres.push_back(sphere);
	//scene.primitives.instances.push_back(instance);

	// BOX //

	ColorTexture boxTexture;
	boxTexture.id = 4;
	boxTexture.color = Color(0.0, 0.0, 1.0);
	boxTexture.intensity = 0.5;

	Material boxMaterial;
	boxMaterial.id = 4;
	boxMaterial.colorTextureId = boxTexture.id;

	Box box;
	box.id = 4;
	box.invisible = true;
	box.materialId = boxMaterial.id;
	box.extent = Vector3(2.0, 2.0, 2.0);

	instance.primitiveId = box.id;
	instance.materialId = box.materialId;
	instance.translate = Vector3(8.0, 1.5, 0.0);
	instance.rotate = EulerAngle(-10.0, 10.0, -10.0);

	scene.textures.colorTextures.push_back(boxTexture);
	scene.materials.push_back(boxMaterial);
	scene.primitives.boxes.push_back(box);
	//scene.primitives.instances.push_back(instance);
	
	// CYLINDER //

	ColorTexture cylinderTexture;
	cylinderTexture.id = 5;
	cylinderTexture.color = Color(1.0, 1.0, 0.0);
	cylinderTexture.intensity = 0.5;

	Material cylinderMaterial;
	cylinderMaterial.id = 5;
	cylinderMaterial.colorTextureId = cylinderTexture.id;

	Cylinder cylinder;
	cylinder.id = 5;
	cylinder.invisible = true;
	cylinder.materialId = cylinderMaterial.id;
	cylinder.radius = 1.0;
	cylinder.height = 2.0;

	instance.primitiveId = cylinder.id;
	instance.materialId = cylinder.materialId;
	instance.translate = Vector3(12.0, 0.5, 0.0);
	instance.rotate = EulerAngle(-10.0, 10.0, 10.0);

	scene.textures.colorTextures.push_back(cylinderTexture);
	scene.materials.push_back(cylinderMaterial);
	scene.primitives.cylinders.push_back(cylinder);
	//scene.primitives.instances.push_back(instance);

	// TORUS //

	ColorTexture torusTexture;
	torusTexture.id = 6;
	torusTexture.color = Color(0.0, 1.0, 1.0);
	torusTexture.intensity = 0.5;

	Material torusMaterial;
	torusMaterial.id = 6;
	torusMaterial.colorTextureId = torusTexture.id;

	Torus torus;
	torus.id = 6;
	torus.invisible = true;
	torus.materialId = torusMaterial.id;
	torus.outerRadius = 1.0;
	torus.innerRadius = 0.25;

	instance.primitiveId = torus.id;
	instance.materialId = torus.materialId;
	instance.translate = Vector3(0.0, 1.25, 0.0);
	instance.rotate = EulerAngle(0.0, 0.0, 0.0);

	scene.textures.colorTextures.push_back(torusTexture);
	scene.materials.push_back(torusMaterial);
	scene.primitives.toruses.push_back(torus);
	scene.primitives.instances.push_back(instance);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.01;

	PointLight pointLight1;
	pointLight1.color = Color(1.0, 1.0, 1.0);
	pointLight1.intensity = 1.5;
	//pointLight1.position = Vector3(6.5, 10.0, 5.0);
	pointLight1.position = Vector3(0.0, 10.0, 5.0);
	pointLight1.distance = 20.0;
	pointLight1.attenuation = 1.0;

	scene.lights.pointLights.push_back(pointLight1);

	return scene;
}
