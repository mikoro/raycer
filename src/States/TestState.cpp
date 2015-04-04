// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "glfw/glfw3.h"

#include "States/TestState.h"
#include "Utils/Log.h"
#include "App/App.h"
#include "Framebuffer/Framebuffer.h"
#include "Raytracing/Raytracer.h"

using namespace Raycer;

TestState::TestState(BaseLog& baseLog, App& app_, Framebuffer& framebuffer_) : app(app_), framebuffer(framebuffer_)
{
	log = baseLog.getNamedLog("TestState");
}

void TestState::initialize()
{

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
