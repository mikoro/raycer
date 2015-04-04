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

TestState::TestState(BaseLog& baseLog, App& app_, Framebuffer& framebuffer_) : app(app_), framebuffer(framebuffer_)
{
	log = baseLog.getNamedLog("TestState");
}

void TestState::initialize()
{
	Material m1;
	m1.color = Color::RED;
	m1.ambientReflectivity = 1.0f;
	m1.diffuseReflectivity = 1.0f;
	m1.specularReflectivity = 1.0f;
	m1.rayReflectivity = 1.0f;
	m1.shininess = 1.0f;

	Sphere* s1 = new Sphere();
	s1->position = Vector3(0.0f, 0.0f, -10.0f);
	s1->radius = 1.0f;
	s1->material = m1;

	Light l1;
	l1.position = Vector3(0.0f, 100.0f, 100.0f);
	l1.diffuseColor = Color::WHITE;
	l1.specularColor = Color::WHITE;

	scene.ambientColor = Color(0.2f, 0.2f, 0.2f);
	scene.camera.position = Vector3(0.0f, 0.0f, 5.0f);
	scene.camera.orientation = EulerAngle(0.0f, 0.0f, 0.0f);
	scene.camera.setFov(90.0f);
	scene.camera.setSize(framebuffer.getWidth(), framebuffer.getHeight());

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

void TestState::update(double timeStep)
{
	(void)timeStep;
}

void TestState::render(double timeStep, double interpolation)
{
	(void)timeStep;
	(void)interpolation;

	Raytracer::raytrace(framebuffer, scene);
}

void TestState::windowResized(int width, int height)
{
	scene.camera.setSize(width, height);
}
