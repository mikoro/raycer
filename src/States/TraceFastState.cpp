// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "glfw/glfw3.h"

#include "States/TraceFastState.h"
#include "Utils/Log.h"
#include "Runners/InteractiveRunner.h"
#include "Rendering/Framebuffer.h"
#include "Runners/InteractiveSettings.h"
#include "CpuRaytracing/Raytracer.h"

//#include "CpuRaytracing/Sphere.h"
//#include "CpuRaytracing/Plane.h"

using namespace Raycer;

TraceFastState::TraceFastState(BaseLog& baseLog, InteractiveRunner& runner_, Framebuffer& framebuffer_, InteractiveSettings& settings_) : runner(runner_), framebuffer(framebuffer_), settings(settings_), scene(baseLog)
{
	log = baseLog.getNamedLog("TraceFastState");
}

void TraceFastState::initialize()
{
	/*
	Material m1;
	m1.color = Color(119, 158, 203);
	m1.diffuseConstant = 1.0;
	m1.specularConstant = 1.0;
	m1.reflectivity = 1.0;
	m1.shininess = 16.0;

	Sphere s1;
	s1.position = Vector3(0.0, 1.0, -5.0);
	s1.setRadius(1.0);
	s1.material = m1;

	m1.color = Color(255, 179, 71);

	Sphere s2;
	s2.position = Vector3(-1.0, 2.0, -5.0);
	s2.setRadius(1.0);
	s2.material = m1;

	m1.color = Color(119, 190, 119);

	Sphere s3;
	s3.position = Vector3(1.0, 2.0, -5.0);
	s3.setRadius(1.0);
	s3.material = m1;

	m1.color = Color(112, 128, 144);
	m1.specularConstant = 1.0;
	m1.shininess = 8.0;

	Plane p1;
	p1.position = Vector3(0.0, 0.0, 0.0);
	p1.normal = Vector3(0.0, 1.0, 0.0);
	p1.material = m1;

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
	scene.camera.initialize(&app, &settings);
	scene.camera.fov = 75.0;
	scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());
	scene.camera.calculateVariables();

	scene.spheres.push_back(s1);
	scene.spheres.push_back(s2);
	scene.spheres.push_back(s3);

	scene.planes.push_back(p1);

	scene.lights.push_back(l1);
	scene.lights.push_back(l2);
	scene.lights.push_back(l3);

	scene.initialize();
	*/

	scene.load("data/scenes/test_scene.json");
	scene.initialize();
	scene.camera.initialize(&runner, &settings);
	scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());
	scene.camera.calculateVariables();
}

void TraceFastState::pause()
{

}

void TraceFastState::resume()
{

}

void TraceFastState::shutdown()
{

}

void TraceFastState::update(double timeStep)
{
	scene.camera.update(timeStep);
}

void TraceFastState::render(double timeStep, double interpolation)
{
	(void)timeStep;
	(void)interpolation;

	pixelCount = 0;
	rayCount = 0;

	Raytracer::traceFast(framebuffer, scene, interrupted, pixelCount, rayCount);

	if (settings.runner.showCameraInfo)
	{
		runner.getInfoFont().drawText(framebuffer, 5, framebuffer.getHeight() - 3 * settings.runner.infoFontSize, tfm::format("Pos: (%.2f, %.2f, %.2f)", scene.camera.position.x, scene.camera.position.y, scene.camera.position.z), Color(255, 255, 255, 200));
		runner.getInfoFont().drawText(framebuffer, 5, framebuffer.getHeight() - 4 * settings.runner.infoFontSize - 2, tfm::format("Rot: (%.2f, %.2f, %.2f)", scene.camera.orientation.yaw, scene.camera.orientation.pitch, scene.camera.orientation.roll), Color(255, 255, 255, 200));
		runner.getInfoFont().drawText(framebuffer, 5, framebuffer.getHeight() - 5 * settings.runner.infoFontSize - 4, tfm::format("Rays: %d", rayCount.load()), Color(255, 255, 255, 200));
	}
}

void TraceFastState::windowResized(int width, int height)
{
	(void)width;
	(void)height;
}

void TraceFastState::framebufferResized(int width, int height)
{
	scene.camera.setImagePlaneSize(width, height);
}
