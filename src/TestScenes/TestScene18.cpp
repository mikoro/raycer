// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Scene.h"
#include "Utils/ObjReader.h"

using namespace Raycer;

// sunflower field
Scene Scene::createTestScene18()
{
	Scene scene;

	scene.raytracer.multiSamples = 0;

	scene.rootBVH.enabled = true;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 5.0, 5.0);
	scene.camera.orientation = EulerAngle(-10.0, 0.0, 0.0);

	// SPHERE 1 //

	ImageTexture sphere1Texture;
	sphere1Texture.id = 1;
	sphere1Texture.intensity = 1.0;
	sphere1Texture.imageFilePath = "data/images/sky.jpg";
	sphere1Texture.applyGamma = true;

	Material sphere1Material;
	sphere1Material.id = 1;
	sphere1Material.colorTextureId = sphere1Texture.id;
	sphere1Material.texcoordScale = Vector2(1.0, 1.0);
	sphere1Material.skipLighting = true;
	sphere1Material.nonShadowing = true;

	Sphere sphere1;
	sphere1.materialId = sphere1Material.id;
	sphere1.position = Vector3(0.0, 0.0, 0.0);
	sphere1.radius = 1000.0;
	sphere1.uvMapType = SphereUVMapType::SPHERICAL;

	scene.textures.imageTextures.push_back(sphere1Texture);
	scene.materials.push_back(sphere1Material);
	scene.primitives.spheres.push_back(sphere1);

	// GROUND //

	ImageTexture groundTexture;
	groundTexture.id = 2;
	groundTexture.intensity = 0.5;
	groundTexture.imageFilePath = "data/images/dirt.jpg";
	groundTexture.applyGamma = true;

	Material groundMaterial;
	groundMaterial.id = 2;
	groundMaterial.colorTextureId = groundTexture.id;
	groundMaterial.texcoordScale = Vector2(5.0, 5.0);

	Plane groundPlane;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();

	scene.textures.imageTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	// MESHES //

	ObjReaderResult result = ObjReader::getMeshes("data/meshes/sunflower/sunflower.obj", Vector3(0.1, 0.1, 0.1), EulerAngle(0.0, 0.0, 0.0), Vector3(0.0, 1.5, 0.0), true, 2000000);
	scene.addObjScene(result);

	Instance instance;
	instance.id = 0;
	instance.primitiveId = result.primitiveGroup.id;
	instance.materialId = result.primitiveGroup.materialId;

	for (int y = -100; y < 100; y += 2)
	{
		for (int x = -100; x < 100; x += 2)
		{
			instance.translate = Vector3(x, 0.0, y);

			scene.primitives.instances.push_back(instance);
		}
	}

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
