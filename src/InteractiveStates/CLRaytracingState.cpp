// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "InteractiveStates/CLRaytracingState.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Runners/InteractiveRunner.h"
#include "Rendering/Framebuffer.h"
#include "CLRaytracing/CLRaytracer.h"

using namespace Raycer;

void CLRaytracingState::initialize()
{
	//Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	CLRaytracer& clRaytracer = App::getCLRaytracer();

	//scene.loadFromFile(settings.scene.fileName);
	scene = Scene::createTestScene();
	scene.initialize();
	scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());

	clRaytracer.initialize();
}

void CLRaytracingState::pause()
{
}

void CLRaytracingState::resume()
{
}

void CLRaytracingState::shutdown()
{
}

void CLRaytracingState::update(double timeStep)
{
	scene.camera.update(timeStep);
}

void CLRaytracingState::render(double timeStep, double interpolation)
{
	(void)timeStep;
	(void)interpolation;

	CLRaytracer& clRaytracer = App::getCLRaytracer();

	scene.camera.interpolate(interpolation);
	scene.camera.precalculate();

	clRaytracer.readScene(scene);
	clRaytracer.uploadData();
	clRaytracer.trace(interrupted);
}

void CLRaytracingState::windowResized(int width, int height)
{
	(void)width;
	(void)height;
}

void CLRaytracingState::framebufferResized(int width, int height)
{
	scene.camera.setImagePlaneSize(width, height);
}
