// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

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

	ColorTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color = Color(1.0, 1.0, 1.0);
	groundTexture.intensity = 0.3;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.ambientMapTextureId = groundTexture.id;
	groundMaterial.diffuseMapTextureId = groundTexture.id;

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
	triangleMaterial.ambientMapTextureId = triangleTexture.id;
	triangleMaterial.diffuseMapTextureId = triangleTexture.id;

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
	scene.primitives.instances.push_back(instance);

	// SPHERE //

	ColorTexture sphereTexture;
	sphereTexture.id = 3;
	sphereTexture.color = Color(0.0, 1.0, 0.0);
	sphereTexture.intensity = 0.5;

	Material sphereMaterial;
	sphereMaterial.id = 3;
	sphereMaterial.ambientMapTextureId = sphereTexture.id;
	sphereMaterial.diffuseMapTextureId = sphereTexture.id;

	Sphere sphere;
	sphere.id = 3;
	sphere.invisible = true;
	sphere.materialId = sphereMaterial.id;
	sphere.radius = 1.0;

	instance.primitiveId = sphere.id;
	instance.materialId = sphere.materialId;
	instance.translate = Vector3(3.0, 1.5, 0.0);

	scene.textures.colorTextures.push_back(sphereTexture);
	scene.materials.push_back(sphereMaterial);
	scene.primitives.spheres.push_back(sphere);
	scene.primitives.instances.push_back(instance);

	// BOX //

	ColorTexture boxTexture;
	boxTexture.id = 4;
	boxTexture.color = Color(0.0, 0.0, 1.0);
	boxTexture.intensity = 0.5;

	Material boxMaterial;
	boxMaterial.id = 4;
	boxMaterial.ambientMapTextureId = boxTexture.id;
	boxMaterial.diffuseMapTextureId = boxTexture.id;

	Box box;
	box.id = 4;
	box.invisible = true;
	box.materialId = boxMaterial.id;
	box.extent = Vector3(2.0, 2.0, 2.0);

	instance.primitiveId = box.id;
	instance.materialId = box.materialId;
	instance.translate = Vector3(6.0, 1.5, 0.0);
	instance.rotate = EulerAngle(10.0, 45.0, -10.0);

	scene.textures.colorTextures.push_back(boxTexture);
	scene.materials.push_back(boxMaterial);
	scene.primitives.boxes.push_back(box);
	scene.primitives.instances.push_back(instance);
	
	// CYLINDER //

	ColorTexture cylinderTexture;
	cylinderTexture.id = 5;
	cylinderTexture.color = Color(1.0, 1.0, 0.0);
	cylinderTexture.intensity = 0.5;

	Material cylinderMaterial;
	cylinderMaterial.id = 5;
	cylinderMaterial.ambientMapTextureId = cylinderTexture.id;
	cylinderMaterial.diffuseMapTextureId = cylinderTexture.id;

	Cylinder cylinder;
	cylinder.id = 5;
	cylinder.invisible = true;
	cylinder.materialId = cylinderMaterial.id;
	cylinder.radius = 1.0;
	cylinder.height = 2.0;

	instance.primitiveId = cylinder.id;
	instance.materialId = cylinder.materialId;
	instance.translate = Vector3(9.0, 0.5, 0.0);
	instance.rotate = EulerAngle(-10.0, 10.0, -10.0);

	scene.textures.colorTextures.push_back(cylinderTexture);
	scene.materials.push_back(cylinderMaterial);
	scene.primitives.cylinders.push_back(cylinder);
	scene.primitives.instances.push_back(instance);

	// TORUS //

	ColorTexture torusTexture;
	torusTexture.id = 6;
	torusTexture.color = Color(0.0, 1.0, 1.0);
	torusTexture.intensity = 0.5;

	Material torusMaterial;
	torusMaterial.id = 6;
	torusMaterial.ambientMapTextureId = torusTexture.id;
	torusMaterial.diffuseMapTextureId = torusTexture.id;

	Torus torus;
	torus.id = 6;
	torus.invisible = true;
	torus.materialId = torusMaterial.id;
	torus.outerRadius = 1.0;
	torus.innerRadius = 0.25;

	instance.primitiveId = torus.id;
	instance.materialId = torus.materialId;
	instance.translate = Vector3(12.0, 1.75, 0.0);
	instance.rotate = EulerAngle(-20.0, -20.0, 0.0);

	scene.textures.colorTextures.push_back(torusTexture);
	scene.materials.push_back(torusMaterial);
	scene.primitives.toruses.push_back(torus);
	scene.primitives.instances.push_back(instance);

	// BLINN BLOB //

	ColorTexture blobTexture;
	blobTexture.id = 7;
	blobTexture.color = Color(1.0, 0.0, 1.0);
	blobTexture.intensity = 0.5;

	Material blobMaterial;
	blobMaterial.id = 7;
	blobMaterial.ambientMapTextureId = blobTexture.id;
	blobMaterial.diffuseMapTextureId = blobTexture.id;

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

	instance.primitiveId = blob.id;
	instance.materialId = blob.materialId;
	instance.translate = Vector3(15.0, 1.5, 0.0);
	instance.rotate = EulerAngle(-20.0, -20.0, 0.0);
	instance.scale = Vector3(2.0, 2.0, 2.0);

	scene.textures.colorTextures.push_back(blobTexture);
	scene.materials.push_back(blobMaterial);
	scene.primitives.blinnBlobs.push_back(blob);
	scene.primitives.instances.push_back(instance);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.01;

	PointLight pointLight1;
	pointLight1.color = Color(1.0, 1.0, 1.0);
	pointLight1.intensity = 1.0;
	pointLight1.position = Vector3(8.0, 10.0, 5.0);
	pointLight1.distance = 20.0;
	pointLight1.attenuation = 1.0;

	scene.lights.pointLights.push_back(pointLight1);

	return scene;
}
