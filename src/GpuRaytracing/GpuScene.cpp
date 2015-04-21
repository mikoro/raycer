// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "GpuRaytracing/GpuScene.h"
#include "Raytracing/Scene.h"

using namespace Raycer;

void GpuScene::readScene(const Scene& scene)
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
		Gpu::Light gpuLight;

		gpuLight.position.x = (float)light.position.x;
		gpuLight.position.y = (float)light.position.y;
		gpuLight.position.z = (float)light.position.z;
		gpuLight.position.w = 0.0f;

		gpuLight.color.x = (float)light.color.r;
		gpuLight.color.y = (float)light.color.g;
		gpuLight.color.z = (float)light.color.b;
		gpuLight.color.w = (float)light.color.a;

		gpuLight.intensity = (float)light.intensity;

		lights.push_back(gpuLight);
	}

	for (const Plane& plane : scene.planes)
	{
		Gpu::Plane gpuPlane;

		gpuPlane.position.x = (float)plane.position.x;
		gpuPlane.position.y = (float)plane.position.y;
		gpuPlane.position.z = (float)plane.position.z;
		gpuPlane.position.w = 0.0f;

		gpuPlane.normal.x = (float)plane.normal.x;
		gpuPlane.normal.y = (float)plane.normal.y;
		gpuPlane.normal.z = (float)plane.normal.z;
		gpuPlane.normal.w = 0.0f;

		gpuPlane.color.x = 1.0f;
		gpuPlane.color.y = 0.0f;
		gpuPlane.color.z = 0.0f;
		gpuPlane.color.w = 1.0f;

		planes.push_back(gpuPlane);
	}

	for (const Sphere& sphere : scene.spheres)
	{
		Gpu::Sphere gpuSphere;

		gpuSphere.position.x = (float)sphere.position.x;
		gpuSphere.position.y = (float)sphere.position.y;
		gpuSphere.position.z = (float)sphere.position.z;
		gpuSphere.position.w = 0.0f;

		gpuSphere.color.x = 0.0f;
		gpuSphere.color.y = 1.0f;
		gpuSphere.color.z = 0.0f;
		gpuSphere.color.w = 1.0f;

		gpuSphere.radius = (float)sphere.radius;

		spheres.push_back(gpuSphere);
	}
}
