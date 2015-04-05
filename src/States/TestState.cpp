// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "glfw/glfw3.h"

#include "States/TestState.h"
#include "Utils/Log.h"
#include "App/App.h"
#include "Framebuffer/Framebuffer.h"
#include "Raytracing/Raytracer.h"
#include "Raytracing/Sphere.h"

using namespace Raycer;

TestState::TestState(BaseLog& baseLog, App& app_, Framebuffer& framebuffer_, Settings& settings_) : app(app_), framebuffer(framebuffer_), settings(settings_)
{
	log = baseLog.getNamedLog("TestState");
}

void TestState::initialize()
{
	Material m1;
	m1.color = Color(65, 105, 225);
	m1.ambientReflectivity = 1.0f;
	m1.diffuseReflectivity = 1.0f;
	m1.specularReflectivity = 1.0f;
	m1.rayReflectivity = 1.0f;
	m1.shininess = 1.0f;

	Sphere* s1 = new Sphere();
	s1->position = Vector3(0.0f, 0.0f, -5.0f);
	s1->setRadius(2.0f);
	s1->material = m1;

	Light l1;
	l1.position = Vector3(0.0f, 100.0f, -5.0f);
	l1.diffuseColor = Color::WHITE;
	l1.specularColor = Color::WHITE;

	scene.ambientColor = Color(0.3f, 0.3f, 0.3f);
	scene.camera.position = Vector3(0.0f, 0.0f, 0.0f);
	scene.camera.orientation = EulerAngle(0.0f, 0.0f, 0.0f);
	scene.camera.initialize(&app, &settings);
	scene.camera.setFov(settings.window.fov);
	scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());

	scene.primitives.push_back(std::shared_ptr<Primitive>(s1));
	scene.lights.push_back(l1);
}

void TestState::pause()
{

}

void TestState::resume()
{

}

void TestState::shutdown()
{

}

void TestState::update(float timeStep)
{
	scene.camera.update(timeStep);
}

void TestState::render(float timeStep, float interpolation)
{
	(void)timeStep;
	(void)interpolation;

	Raytracer::raytrace(framebuffer, scene);
}

void TestState::windowResized(int width, int height)
{
	(void)width;
	(void)height;
}

void TestState::framebufferResized(int width, int height)
{
	scene.camera.setImagePlaneSize(width, height);
}
