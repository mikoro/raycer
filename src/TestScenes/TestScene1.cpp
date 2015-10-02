// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// simple triangle, sphere and box on a plane with one point light
Scene Scene::createTestScene1()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	scene.rootBVH.enabled = true;
	scene.rootBVH.buildInfo.maxLeafSize = 1;

	// CAMERA //

	scene.camera.position = Vector3(8.0, 8.0, 11.0);
	scene.camera.orientation = EulerAngle(-30.0, 0.0, 0.0);

	// GROUND //

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.ambientReflectance = Color(1.0, 1.0, 1.0) * 0.3;
	groundMaterial.diffuseReflectance = groundMaterial.ambientReflectance;

	Plane groundPlane;
	groundPlane.id = 1;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();

	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// TRIANGLE //

	Material triangleMaterial;
	triangleMaterial.id = 2;
	triangleMaterial.ambientReflectance = Color(1.0, 0.0, 0.0) * 0.5;
	triangleMaterial.diffuseReflectance = triangleMaterial.ambientReflectance;

	Triangle triangle;
	triangle.id = 2;
	triangle.invisible = true;
	triangle.materialId = triangleMaterial.id;
	triangle.vertices[0] = Vector3(-1.0, 0.0, 0.0);
	triangle.vertices[1] = Vector3(1.0, 0.0, 0.0);
	triangle.vertices[2] = Vector3(0.0, 2.0, 0.0);
	triangle.normals[0] = triangle.normals[1] = triangle.normals[2] = Vector3::FORWARD;

	Instance instance;
	instance.id = 1002;
	instance.primitiveId = triangle.id;
	instance.translate = Vector3(0.0, 0.5, 0.0);
	instance.rotate = EulerAngle(-10.0, 10.0, 10.0);

	scene.materials.push_back(triangleMaterial);
	scene.primitives.triangles.push_back(triangle);
	scene.primitives.instances.push_back(instance);

	// SPHERE //

	Material sphereMaterial;
	sphereMaterial.id = 3;
	sphereMaterial.ambientReflectance = Color(0.0, 1.0, 0.0) * 0.5;
	sphereMaterial.diffuseReflectance = sphereMaterial.ambientReflectance;

	Sphere sphere;
	sphere.id = 3;
	sphere.invisible = true;
	sphere.materialId = sphereMaterial.id;
	sphere.radius = 1.0;

	instance.id = 1003;
	instance.primitiveId = sphere.id;
	instance.translate = Vector3(3.0, 1.5, 0.0);

	scene.materials.push_back(sphereMaterial);
	scene.primitives.spheres.push_back(sphere);
	scene.primitives.instances.push_back(instance);

	// BOX //

	Material boxMaterial;
	boxMaterial.id = 4;
	boxMaterial.ambientReflectance = Color(0.0, 0.0, 1.0) * 0.5;
	boxMaterial.diffuseReflectance = boxMaterial.ambientReflectance;

	Box box;
	box.id = 4;
	box.invisible = true;
	box.materialId = boxMaterial.id;
	box.extent = Vector3(2.0, 2.0, 2.0);

	instance.id = 1004;
	instance.primitiveId = box.id;
	instance.translate = Vector3(6.0, 1.5, 0.0);
	instance.rotate = EulerAngle(10.0, 45.0, -10.0);

	scene.materials.push_back(boxMaterial);
	scene.primitives.boxes.push_back(box);
	scene.primitives.instances.push_back(instance);

	// CYLINDER //

	Material cylinderMaterial;
	cylinderMaterial.id = 5;
	cylinderMaterial.ambientReflectance = Color(1.0, 1.0, 0.0) * 0.5;
	cylinderMaterial.diffuseReflectance = cylinderMaterial.ambientReflectance;

	Cylinder cylinder;
	cylinder.id = 5;
	cylinder.invisible = true;
	cylinder.materialId = cylinderMaterial.id;
	cylinder.radius = 1.0;
	cylinder.height = 2.0;

	instance.id = 1005;
	instance.primitiveId = cylinder.id;
	instance.translate = Vector3(9.0, 0.5, 0.0);
	instance.rotate = EulerAngle(-10.0, 10.0, -10.0);

	scene.materials.push_back(cylinderMaterial);
	scene.primitives.cylinders.push_back(cylinder);
	scene.primitives.instances.push_back(instance);

	// TORUS //

	Material torusMaterial;
	torusMaterial.id = 6;
	torusMaterial.ambientReflectance = Color(0.0, 1.0, 1.0) * 0.5;
	torusMaterial.diffuseReflectance = torusMaterial.ambientReflectance;

	Torus torus;
	torus.id = 6;
	torus.invisible = true;
	torus.materialId = torusMaterial.id;
	torus.outerRadius = 1.0;
	torus.innerRadius = 0.25;

	instance.id = 1006;
	instance.primitiveId = torus.id;
	instance.translate = Vector3(12.0, 1.75, 0.0);
	instance.rotate = EulerAngle(-20.0, -20.0, 0.0);

	scene.materials.push_back(torusMaterial);
	scene.primitives.toruses.push_back(torus);
	scene.primitives.instances.push_back(instance);

	// BLINN BLOB //

	Material blobMaterial;
	blobMaterial.id = 7;
	blobMaterial.ambientReflectance = Color(1.0, 0.0, 1.0) * 0.5;
	blobMaterial.diffuseReflectance = blobMaterial.ambientReflectance;

	BlinnBlob blob;
	blob.id = 7;
	blob.invisible = true;
	blob.materialId = blobMaterial.id;

	BlinnBlobDescription blobDesc;
	blobDesc.position = Vector3(0.0, 0.0, 0.0);
	blobDesc.radius = 0.4;
	blob.blobs.push_back(blobDesc);
	blobDesc.position = Vector3(0.0, 1.0, 0.1);
	blobDesc.radius = 0.4;
	blob.blobs.push_back(blobDesc);
	blobDesc.position = Vector3(-0.5, 0.5, 0.2);
	blobDesc.radius = 0.3;
	blob.blobs.push_back(blobDesc);

	instance.id = 1007;
	instance.primitiveId = blob.id;
	instance.translate = Vector3(15.0, 1.5, 0.0);
	instance.rotate = EulerAngle(-20.0, -20.0, 0.0);
	instance.scale = Vector3(2.0, 2.0, 2.0);

	scene.materials.push_back(blobMaterial);
	scene.primitives.blinnBlobs.push_back(blob);
	scene.primitives.instances.push_back(instance);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.01;

	PointLight pointLight;
	pointLight.color = Color(1.0, 1.0, 1.0);
	pointLight.intensity = 1.5;
	pointLight.position = Vector3(8.0, 10.0, 5.0);
	pointLight.maxDistance = 20.0;
	pointLight.attenuation = 1.0;

	scene.lights.pointLights.push_back(pointLight);

	return scene;
}
