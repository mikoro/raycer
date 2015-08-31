// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"

using namespace Raycer;

// constructive solid geometry
Scene Scene::createTestScene17()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(-1.71, 2.41, 2.34);
	scene.camera.orientation = EulerAngle(-26.44, -32.0, 0.0);

	// GROUND //

	CheckerTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color1 = Color(0.1, 0.1, 0.1);
	groundTexture.color2 = Color(0.4, 0.4, 0.4);
	groundTexture.intensity = 1.0;
	groundTexture.stripeMode = true;
	groundTexture.stripeWidth = 0.05;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.colorTextureId = groundTexture.id;
	groundMaterial.texcoordScale = Vector2(2.0, 2.0);

	Plane groundPlane;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();

	scene.textures.checkerTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// MATERIALS AND TEXTURES //

	MarbleTexture marbleTexture;
	marbleTexture.id = 2;
	marbleTexture.intensity = 0.5;

	Material marbleMaterial;
	marbleMaterial.id = 2;
	marbleMaterial.colorTextureId = marbleMaterial.id;

	WoodTexture woodTexture;
	woodTexture.id = 3;
	woodTexture.intensity = 0.5;

	Material woodMaterial;
	woodMaterial.id = 3;
	woodMaterial.colorTextureId = woodMaterial.id;

	FireTexture fireTexture;
	fireTexture.id = 4;
	fireTexture.intensity = 0.5;

	Material fireMaterial;
	fireMaterial.id = 4;
	fireMaterial.colorTextureId = fireMaterial.id;

	scene.textures.marbleTextures.push_back(marbleTexture);
	scene.textures.woodTextures.push_back(woodTexture);
	scene.textures.fireTextures.push_back(fireTexture);

	scene.materials.push_back(marbleMaterial);
	scene.materials.push_back(woodMaterial);
	scene.materials.push_back(fireMaterial);

	// BASE //

	Box box1;
	box1.id = 1;
	box1.invisible = true;
	box1.materialId = woodMaterial.id;
	box1.position = Vector3(0.0, 0.5, 0.0);
	box1.extent = Vector3(1.0, 1.0, 1.0);

	scene.primitives.boxes.push_back(box1);

	Sphere sphere1;
	sphere1.id = 2;
	sphere1.invisible = true;
	sphere1.materialId = woodMaterial.id;
	sphere1.position = Vector3(0.0, 0.5, 0.0);
	sphere1.radius = 0.65;

	scene.primitives.spheres.push_back(sphere1);

	CSG csg1;
	csg1.id = 3;
	csg1.materialId = woodMaterial.id;
	csg1.operation = CSGOperation::DIFFERENCE;
	csg1.leftPrimitiveId = 1;
	csg1.rightPrimitiveId = 2;

	scene.primitives.csgs.push_back(csg1);

	// VASE //

	Sphere sphere2;
	sphere2.id = 4;
	sphere2.invisible = true;
	sphere2.materialId = marbleMaterial.id;
	sphere2.position = Vector3(0.0, 1.48, 0.0);
	sphere2.radius = 0.65;

	scene.primitives.spheres.push_back(sphere2);

	Box box2;
	box2.id = 5;
	box2.invisible = true;
	box2.materialId = marbleMaterial.id;
	box2.position = Vector3(0.0, 2.5, 0.0);
	box2.extent = Vector3(2.0, 2.0, 2.0);

	scene.primitives.boxes.push_back(box2);

	CSG csg2;
	csg2.id = 6;
	csg2.invisible = true;
	csg2.materialId = marbleMaterial.id;
	csg2.operation = CSGOperation::DIFFERENCE;
	csg2.leftPrimitiveId = 4;
	csg2.rightPrimitiveId = 5;

	scene.primitives.csgs.push_back(csg2);

	Sphere sphere3;
	sphere3.id = 7;
	sphere3.invisible = true;
	sphere3.materialId = marbleMaterial.id;
	sphere3.position = Vector3(0.0, 1.5, 0.0);
	sphere3.radius = 0.5;

	scene.primitives.spheres.push_back(sphere3);

	CSG csg3;
	csg3.id = 8;
	csg3.invisible = true;
	csg3.materialId = marbleMaterial.id;
	csg3.operation = CSGOperation::DIFFERENCE;
	csg3.leftPrimitiveId = 6;
	csg3.rightPrimitiveId = 7;

	scene.primitives.csgs.push_back(csg3);

	Cylinder cylinder1;
	cylinder1.id = 9;
	cylinder1.invisible = true;
	cylinder1.materialId = marbleMaterial.id;
	cylinder1.radius = 0.15;
	cylinder1.height = 2.0;

	scene.primitives.cylinders.push_back(cylinder1);

	Instance instance1;
	instance1.id = 10;
	instance1.primitiveId = 9;
	instance1.invisible = true;
	instance1.materialId = marbleMaterial.id;
	instance1.rotate = EulerAngle(-90.0, 0.0, 0.0);
	instance1.translate = Vector3(0.0, 0.25, 0.0);

	scene.primitives.instances.push_back(instance1);

	Instance instance2;
	instance2.id = 11;
	instance2.primitiveId = 9;
	instance2.invisible = true;
	instance2.materialId = marbleMaterial.id;
	instance2.rotate = EulerAngle(-90.0, 0.0, 90.0);
	instance2.translate = Vector3(0.0, 0.25, 0.0);

	scene.primitives.instances.push_back(instance2);

	CSG csg4;
	csg4.id = 12;
	csg4.invisible = true;
	csg4.materialId = marbleMaterial.id;
	csg4.operation = CSGOperation::UNION;
	csg4.leftPrimitiveId = 10;
	csg4.rightPrimitiveId = 11;

	scene.primitives.csgs.push_back(csg4);

	CSG csg5;
	csg5.id = 13;
	csg5.materialId = marbleMaterial.id;
	csg5.operation = CSGOperation::DIFFERENCE;
	csg5.leftPrimitiveId = 8;
	csg5.rightPrimitiveId = 12;

	scene.primitives.csgs.push_back(csg5);

	// FIRE //

	Sphere sphere4;
	sphere4.id = 14;
	sphere4.invisible = true;
	sphere4.materialId = fireMaterial.id;
	sphere4.position = Vector3(0.25, 1.5, 0.0);
	sphere4.radius = 0.5;

	scene.primitives.spheres.push_back(sphere4);

	Sphere sphere5;
	sphere5.id = 15;
	sphere5.invisible = true;
	sphere5.materialId = fireMaterial.id;
	sphere5.position = Vector3(-0.25, 1.5, 0.0);
	sphere5.radius = 0.5;

	scene.primitives.spheres.push_back(sphere5);

	Sphere sphere6;
	sphere6.id = 16;
	sphere6.invisible = true;
	sphere6.materialId = fireMaterial.id;
	sphere6.position = Vector3(0.0, 1.5, 0.25);
	sphere6.radius = 0.5;

	scene.primitives.spheres.push_back(sphere6);

	Sphere sphere7;
	sphere7.id = 17;
	sphere7.invisible = true;
	sphere7.materialId = fireMaterial.id;
	sphere7.position = Vector3(0.0, 1.5, -0.25);
	sphere7.radius = 0.5;

	scene.primitives.spheres.push_back(sphere7);

	CSG csg6;
	csg6.id = 18;
	csg6.invisible = true;
	csg6.materialId = marbleMaterial.id;
	csg6.operation = CSGOperation::INTERSECTION;
	csg6.leftPrimitiveId = 14;
	csg6.rightPrimitiveId = 15;

	scene.primitives.csgs.push_back(csg6);

	CSG csg7;
	csg7.id = 19;
	csg7.invisible = true;
	csg7.materialId = marbleMaterial.id;
	csg7.operation = CSGOperation::INTERSECTION;
	csg7.leftPrimitiveId = 18;
	csg7.rightPrimitiveId = 16;

	scene.primitives.csgs.push_back(csg7);

	CSG csg8;
	csg8.id = 20;
	csg8.invisible = true;
	csg8.materialId = marbleMaterial.id;
	csg8.operation = CSGOperation::INTERSECTION;
	csg8.leftPrimitiveId = 19;
	csg8.rightPrimitiveId = 17;

	scene.primitives.csgs.push_back(csg8);

	Instance instance3;
	instance3.id = 21;
	instance3.primitiveId = 20;
	instance3.materialId = fireMaterial.id;
	instance3.scale = Vector3(1.0, 1.5, 1.0);

	scene.primitives.instances.push_back(instance3);

	// LIGHTS //

	scene.lights.ambientLight.color = Color(1.0, 1.0, 1.0);
	scene.lights.ambientLight.intensity = 0.1;

	DirectionalLight directionalLight1;
	directionalLight1.color = Color(1.0, 1.0, 1.0);
	directionalLight1.intensity = 1.0;
	directionalLight1.direction = EulerAngle(-30.0, 30.0, 0.0).getDirection();

	scene.lights.directionalLights.push_back(directionalLight1);

	return scene;
}
