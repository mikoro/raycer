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

	scene.camera.position = Vector3(0.0, 3.0, 3.0);
	scene.camera.orientation = EulerAngle(-30.0, 0.0, 0.0);

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

	// SPHERE 1 //

	MarbleTexture sphere1Texture;
	sphere1Texture.id = 2;
	sphere1Texture.intensity = 0.5;

	Material sphere1Material;
	sphere1Material.id = 2;
	sphere1Material.colorTextureId = sphere1Texture.id;

	Sphere sphere1;
	sphere1.id = 1;
	sphere1.invisible = true;
	sphere1.materialId = sphere1Material.id;
	sphere1.position = Vector3(0.0, 1.0, 0.0);
	sphere1.radius = 1.0;

	scene.textures.marbleTextures.push_back(sphere1Texture);
	scene.materials.push_back(sphere1Material);
	scene.primitives.spheres.push_back(sphere1);

	// SPHERE 2 //

	Sphere sphere2;
	sphere2.id = 2;
	sphere2.invisible = true;
	sphere2.materialId = sphere1Material.id;
	sphere2.position = Vector3(0.0, 1.3, 0.0);
	sphere2.radius = 0.8;

	scene.primitives.spheres.push_back(sphere2);

	// BOX 1 //

	Box box1;
	box1.id = 3;
	box1.invisible = true;
	box1.materialId = sphere1Material.id;
	box1.position = Vector3(0.0, 2.5, 0.0);
	box1.extent = Vector3(2.0, 2.0, 2.0);
	
	scene.primitives.boxes.push_back(box1);

	// CSG 1

	CSG csg1;
	csg1.id = 4;
	csg1.invisible = true;
	csg1.materialId = 1;
	csg1.operation = CSGOperation::DIFFERENCE;
	csg1.leftPrimitiveId = 1;
	csg1.rightPrimitiveId = 2;

	scene.primitives.csgs.push_back(csg1);

	// CSG 2

	CSG csg2;
	csg2.id = 5;
	csg2.materialId = 1;
	csg2.operation = CSGOperation::DIFFERENCE;
	csg2.leftPrimitiveId = 4;
	csg2.rightPrimitiveId = 3;

	scene.primitives.csgs.push_back(csg2);

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
