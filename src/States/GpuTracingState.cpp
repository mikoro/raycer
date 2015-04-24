// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "States/GpuTracingState.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Runners/InteractiveRunner.h"
#include "Rendering/Framebuffer.h"
#include "CLRaytracing/CLRaytracer.h"

using namespace Raycer;

void GpuTracingState::initialize()
{
	Framebuffer& framebuffer = App::getFramebuffer();
	CLRaytracer& gpuRaytracer = App::getGpuRaytracer();

	scene.loadFromFile("data/scenes/test_scene.json");
	scene.initialize();
	scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());

	gpuRaytracer.initialize();
}

void GpuTracingState::pause()
{
}

void GpuTracingState::resume()
{
}

void GpuTracingState::shutdown()
{
}

void GpuTracingState::update(double timeStep)
{
	scene.camera.update(timeStep);
}

void GpuTracingState::render(double timeStep, double interpolation)
{
	(void)timeStep;
	(void)interpolation;

	CLRaytracer& gpuRaytracer = App::getGpuRaytracer();

	scene.camera.interpolate(interpolation);
	scene.camera.precalculate();

	gpuRaytracer.readScene(scene);
	gpuRaytracer.uploadData();
	gpuRaytracer.trace(interrupted);
}

void GpuTracingState::windowResized(int width, int height)
{
	(void)width;
	(void)height;
}

void GpuTracingState::framebufferResized(int width, int height)
{
	scene.camera.setImagePlaneSize(width, height);
}
