// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "States/GpuTracingState.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Runners/InteractiveRunner.h"
#include "Rendering/Framebuffer.h"
#include "GpuRaytracing/GpuRaytracer.h"

using namespace Raycer;

void GpuTracingState::initialize()
{
	Framebuffer& framebuffer = App::getFramebuffer();

	scene.loadFromFile("data/scenes/test_scene.json");
	scene.initialize();
	scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());
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

	Framebuffer& framebuffer = App::getFramebuffer();
	GpuRaytracer& gpuRaytracer = App::getGpuRaytracer();

	scene.camera.interpolate(interpolation);
	scene.camera.precalculate();

	config.renderTarget = nullptr;
	config.scene = &scene;
	config.sceneWidth = framebuffer.getWidth();
	config.sceneHeight = framebuffer.getHeight();
	config.pixelOffset = 0;
	config.pixelCount = config.sceneWidth * config.sceneHeight;
	config.pixelsProcessed = 0;
	config.raysProcessed = 0;
	config.isInteractive = true;

	gpuRaytracer.trace(config, interrupted);
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
