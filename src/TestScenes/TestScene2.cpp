// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Raytracing/Scene.h"

using namespace Raycer;

// only a checkerboard plane
Scene Scene::createTestScene2()
{
	Scene scene;

	scene.general.multiSamplerType = SamplerType::CMJ;
	scene.general.multiSamplerFilterType = FilterType::MITCHELL;
	scene.general.multiSamples = 0;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 0.5, 0.0);
	scene.camera.orientation = EulerAngle(-22.0, 0.0, 0.0);

	// GROUND //

	CheckerTexture groundTexture;
	groundTexture.id = 1;
	groundTexture.color1 = Color(1.0, 1.0, 1.0);
	groundTexture.color2 = Color(0.0, 0.0, 0.0);
	groundTexture.intensity = 1.0;

	Material groundMaterial;
	groundMaterial.id = 1;
	groundMaterial.ambientMapTextureId = groundTexture.id;
	groundMaterial.diffuseMapTextureId = groundTexture.id;
	groundMaterial.emittanceMapTextureId = groundTexture.id;
	groundMaterial.skipLighting = true;
	groundMaterial.texcoordScale = Vector2(2.0, 2.0);

	Plane groundPlane;
	groundPlane.id = 1;
	groundPlane.materialId = groundMaterial.id;
	groundPlane.position = Vector3(0.0, 0.0, 0.0);
	groundPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();

	scene.textures.checkerTextures.push_back(groundTexture);
	scene.materials.push_back(groundMaterial);
	scene.primitives.planes.push_back(groundPlane);

	return scene;
}
