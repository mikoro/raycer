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

	scene.camera.position = Vector3(0.0, 5.0, 7.0);
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

	ColorTexture sphere1Texture;
	sphere1Texture.id = 2;
	sphere1Texture.color = Color(1.0, 0.0, 0.0);
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

	scene.textures.colorTextures.push_back(sphere1Texture);
	scene.materials.push_back(sphere1Material);

	// SPHERE 2 //

	ColorTexture sphere2Texture;
	sphere2Texture.id = 3;
	sphere2Texture.color = Color(0.0, 1.0, 0.0);
	sphere2Texture.intensity = 0.5;

	Material sphere2Material;
	sphere2Material.id = 3;
	sphere2Material.colorTextureId = sphere2Texture.id;

	Sphere sphere2;
	sphere2.id = 2;
	sphere2.invisible = true;
	sphere2.materialId = sphere2Material.id;
	sphere2.position = Vector3(1.0, 1.0, 0.0);
	sphere2.radius = 1.0;

	scene.textures.colorTextures.push_back(sphere2Texture);
	scene.materials.push_back(sphere2Material);

	scene.primitives.spheres.push_back(sphere1);
	scene.primitives.spheres.push_back(sphere2);

	// BOX 1 //

	ColorTexture box1Texture;
	box1Texture.id = 4;
	box1Texture.color = Color(0.0, 0.0, 1.0);
	box1Texture.intensity = 0.5;

	Material box1Material;
	box1Material.id = 4;
	box1Material.colorTextureId = box1Texture.id;

	Box box1;
	box1.id = 3;
	box1.invisible = true;
	box1.position = Vector3(0.0, 2.0, 0.0);
	box1.extent = Vector3(2.0, 2.0, 2.0);
	box1.materialId = box1Material.id;

	scene.textures.colorTextures.push_back(box1Texture);
	scene.materials.push_back(box1Material);
	scene.primitives.boxes.push_back(box1);

	// CSG

	CSG csg1;
	csg1.materialId = 1;
	csg1.operation = CSGOperation::SUBSTRACTION;
	csg1.leftPrimitiveId = 1;
	csg1.rightPrimitiveId = 2;

	scene.primitives.csgs.push_back(csg1);

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
