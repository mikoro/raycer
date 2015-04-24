// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "CLRaytracing/CLScene.h"
#include "Raytracing/Scene.h"

using namespace Raycer;

void CLScene::readScene(const Scene& scene)
{
	camera.position.x = (float)scene.camera.position.x;
	camera.position.y = (float)scene.camera.position.y;
	camera.position.z = (float)scene.camera.position.z;
	camera.position.w = 0.0f;

	camera.up.x = (float)scene.camera.up.x;
	camera.up.y = (float)scene.camera.up.y;
	camera.up.z = (float)scene.camera.up.z;
	camera.up.w = 0.0f;

	camera.right.x = (float)scene.camera.right.x;
	camera.right.y = (float)scene.camera.right.y;
	camera.right.z = (float)scene.camera.right.z;
	camera.right.w = 0.0f;

	camera.imagePlaneCenter.x = (float)scene.camera.imagePlaneCenter.x;
	camera.imagePlaneCenter.y = (float)scene.camera.imagePlaneCenter.y;
	camera.imagePlaneCenter.z = (float)scene.camera.imagePlaneCenter.z;
	camera.imagePlaneCenter.w = 0.0f;

	camera.imagePlaneWidth = (float)scene.camera.imagePlaneWidth;
	camera.imagePlaneHeight = (float)scene.camera.imagePlaneHeight;
	camera.aspectRatio = (float)scene.camera.aspectRatio;

	lights.clear();
	planes.clear();
	spheres.clear();

	for (const Light& light : scene.lights)
	{
		OpenCL::Light clLight;

		clLight.position.x = (float)light.position.x;
		clLight.position.y = (float)light.position.y;
		clLight.position.z = (float)light.position.z;
		clLight.position.w = 0.0f;

		clLight.color.x = (float)light.color.r;
		clLight.color.y = (float)light.color.g;
		clLight.color.z = (float)light.color.b;
		clLight.color.w = (float)light.color.a;

		clLight.intensity = (float)light.intensity;

		lights.push_back(clLight);
	}

	for (const Plane& plane : scene.planes)
	{
		OpenCL::Plane clPlane;

		clPlane.position.x = (float)plane.position.x;
		clPlane.position.y = (float)plane.position.y;
		clPlane.position.z = (float)plane.position.z;
		clPlane.position.w = 0.0f;

		clPlane.normal.x = (float)plane.normal.x;
		clPlane.normal.y = (float)plane.normal.y;
		clPlane.normal.z = (float)plane.normal.z;
		clPlane.normal.w = 0.0f;

		clPlane.color.x = 1.0f;
		clPlane.color.y = 0.0f;
		clPlane.color.z = 0.0f;
		clPlane.color.w = 1.0f;

		planes.push_back(clPlane);
	}

	for (const Sphere& sphere : scene.spheres)
	{
		OpenCL::Sphere clSphere;

		clSphere.position.x = (float)sphere.position.x;
		clSphere.position.y = (float)sphere.position.y;
		clSphere.position.z = (float)sphere.position.z;
		clSphere.position.w = 0.0f;

		clSphere.color.x = 0.0f;
		clSphere.color.y = 1.0f;
		clSphere.color.z = 0.0f;
		clSphere.color.w = 1.0f;

		clSphere.radius = (float)sphere.radius;

		spheres.push_back(clSphere);
	}
}
