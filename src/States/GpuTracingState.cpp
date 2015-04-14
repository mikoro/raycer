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

	scene.load("data/scenes/test_scene.json");
	scene.initialize();
	scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());
	scene.camera.calculateVariables();
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

	info.renderTarget = nullptr;
	info.scene = &scene;
	info.sceneWidth = framebuffer.getWidth();
	info.sceneHeight = framebuffer.getHeight();
	info.pixelStartOffset = 0;
	info.pixelTotalCount = info.sceneWidth * info.sceneHeight;
	info.pixelsProcessed = 0;
	info.raysProcessed = 0;

	gpuRaytracer.trace(info, interrupted);
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
