// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "tinyformat/tinyformat.h"

#include "Utils/ConsoleRunner.h"
#include "Rendering/Image.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Raytracer.h"
#include "Raytracing/Material.h"
#include "Raytracing/Sphere.h"
#include "Raytracing/Plane.h"

using namespace Raycer;

namespace
{
	Scene createScene(int width, int height)
	{
		Scene scene;
		Image image = Image(width, height);

		Material m1;
		m1.color = Color(119, 158, 203);
		m1.diffuseConstant = 1.0;
		m1.specularConstant = 1.0;
		m1.reflectivity = 1.0;
		m1.shininess = 16.0;

		Sphere* s1 = new Sphere();
		s1->position = Vector3(0.0, 1.0, -5.0);
		s1->setRadius(1.0);
		s1->material = m1;

		m1.color = Color(255, 179, 71);

		Sphere* s2 = new Sphere();
		s2->position = Vector3(-1.0, 2.0, -5.0);
		s2->setRadius(1.0);
		s2->material = m1;

		m1.color = Color(119, 190, 119);

		Sphere* s3 = new Sphere();
		s3->position = Vector3(1.0, 2.0, -5.0);
		s3->setRadius(1.0);
		s3->material = m1;

		m1.color = Color(112, 128, 144);
		m1.specularConstant = 1.0;
		m1.shininess = 8.0;

		Plane* p1 = new Plane();
		p1->position = Vector3(0.0, 0.0, 0.0);
		p1->normal = Vector3(0.0, 1.0, 0.0);
		p1->material = m1;

		Light l1;
		l1.position = Vector3(0.0, 10.0, -5.0);
		l1.color = Color(1.0, 1.0, 1.0);
		l1.intensity = 0.5;

		Light l2;
		l2.position = Vector3(0.0, 4.0, 10.0);
		l2.color = Color(1.0, 1.0, 1.0);
		l2.intensity = 0.5;

		Light l3;
		l3.position = Vector3(0.0, -10.0, -5.0);
		l3.color = Color(1.0, 1.0, 1.0);
		l3.intensity = 0.5;

		scene.maxReflections = 1;

		scene.fogEnabled = true;
		scene.fogDistance = 40.0;
		scene.fogSteepness = 4.0;
		scene.fogColor = Color(128, 128, 128);

		scene.camera.position = Vector3(0.0, 1.0, 0.0);
		scene.camera.orientation = EulerAngle(0.0, 0.0, 0.0);
		scene.camera.setFov(75.0);
		scene.camera.setImagePlaneSize(width, height);
		scene.camera.calculateVariables();

		scene.primitives.push_back(std::shared_ptr<Primitive>(s1));
		scene.primitives.push_back(std::shared_ptr<Primitive>(s2));
		scene.primitives.push_back(std::shared_ptr<Primitive>(s3));
		scene.primitives.push_back(std::shared_ptr<Primitive>(p1));

		scene.lights.push_back(l1);
		scene.lights.push_back(l2);
		scene.lights.push_back(l3);

		return scene;
	}
}

int ConsoleRunner::run(const ConsoleRunnerSettings& settings)
{
	Image image = Image(settings.width, settings.height);
	Scene scene = createScene(settings.width, settings.height);

	Raytracer::traceFast(image, scene);

	image.saveAs(settings.outputFileName);

	return 0;
}
