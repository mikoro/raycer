// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "OpenCL/CLScene.h"
//#include "Raytracing/Scene.h"

using namespace Raycer;

void CLScene::convertSceneData(const Scene& scene)
{
	(void)scene;

	/*
	camera.position.s[0]= (float)scene.camera.position.x;
	camera.position.s[1]= (float)scene.camera.position.y;
	camera.position.s[2]= (float)scene.camera.position.z;
	camera.position.s[3]= 0.0f;

	camera.up.s[0]= (float)scene.camera.up.x;
	camera.up.s[1]= (float)scene.camera.up.y;
	camera.up.s[2]= (float)scene.camera.up.z;
	camera.up.s[3]= 0.0f;

	camera.right.s[0]= (float)scene.camera.right.x;
	camera.right.s[1]= (float)scene.camera.right.y;
	camera.right.s[2]= (float)scene.camera.right.z;
	camera.right.s[3]= 0.0f;

	camera.imagePlaneCenter.s[0]= (float)scene.camera.imagePlaneCenter.x;
	camera.imagePlaneCenter.s[1]= (float)scene.camera.imagePlaneCenter.y;
	camera.imagePlaneCenter.s[2]= (float)scene.camera.imagePlaneCenter.z;
	camera.imagePlaneCenter.s[3]= 0.0f;

	camera.imagePlaneWidth = (float)scene.camera.imagePlaneWidth;
	camera.imagePlaneHeight = (float)scene.camera.imagePlaneHeight;
	camera.aspectRatio = (float)scene.camera.aspectRatio;

	lights.clear();
	planes.clear();
	spheres.clear();

	for (const Light& light : scene.lights)
	{
		OpenCL::Light clLight;

		clLight.position.s[0]= (float)light.position.x;
		clLight.position.s[1]= (float)light.position.y;
		clLight.position.s[2]= (float)light.position.z;
		clLight.position.s[3]= 0.0f;

		clLight.color.s[0]= (float)light.color.r;
		clLight.color.s[1]= (float)light.color.g;
		clLight.color.s[2]= (float)light.color.b;
		clLight.color.s[3]= (float)light.color.a;

		clLight.intensity = (float)light.intensity;

		lights.push_back(clLight);
	}

	for (const Plane& plane : scene.planes)
	{
		OpenCL::Plane clPlane;

		clPlane.position.s[0]= (float)plane.position.x;
		clPlane.position.s[1]= (float)plane.position.y;
		clPlane.position.s[2]= (float)plane.position.z;
		clPlane.position.s[3]= 0.0f;

		clPlane.normal.s[0]= (float)plane.normal.x;
		clPlane.normal.s[1]= (float)plane.normal.y;
		clPlane.normal.s[2]= (float)plane.normal.z;
		clPlane.normal.s[3]= 0.0f;

		clPlane.color.s[0]= 1.0f;
		clPlane.color.s[1]= 0.0f;
		clPlane.color.s[2]= 0.0f;
		clPlane.color.s[3]= 1.0f;

		planes.push_back(clPlane);
	}

	for (const Sphere& sphere : scene.spheres)
	{
		OpenCL::Sphere clSphere;

		clSphere.position.s[0]= (float)sphere.position.x;
		clSphere.position.s[1]= (float)sphere.position.y;
		clSphere.position.s[2]= (float)sphere.position.z;
		clSphere.position.s[3]= 0.0f;

		clSphere.color.s[0]= 0.0f;
		clSphere.color.s[1]= 1.0f;
		clSphere.color.s[2]= 0.0f;
		clSphere.color.s[3]= 1.0f;

		clSphere.radius = (float)sphere.radius;

		spheres.push_back(clSphere);
	}
	*/
}
