// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "glfw/glfw3.h"

#include "States/CpuTracingState.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Runners/InteractiveRunner.h"
#include "Rendering/Framebuffer.h"
#include "CpuRaytracing/CpuRaytracer.h"

using namespace Raycer;

void CpuTracingState::initialize()
{
	Framebuffer& framebuffer = App::getFramebuffer();

	scene.load("data/scenes/test_scene.json");
	scene.initialize();
	scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());
	scene.camera.calculateVariables();
}

void CpuTracingState::pause()
{

}

void CpuTracingState::resume()
{

}

void CpuTracingState::shutdown()
{

}

void CpuTracingState::update(double timeStep)
{
	scene.camera.update(timeStep);
}

void CpuTracingState::render(double timeStep, double interpolation)
{
	(void)timeStep;
	(void)interpolation;

	Framebuffer& framebuffer = App::getFramebuffer();
	Settings& settings = App::getSettings();
	CpuRaytracer& cpuRaytracer = App::getCpuRaytracer();
	Font& infoFont = App::getInteractiveRunner().getInfoFont();

	pixelCount = 0;
	rayCount = 0;

	cpuRaytracer.trace(framebuffer, scene, interrupted, pixelCount, rayCount);

	if (settings.interactive.showCameraInfo)
	{
		infoFont.drawText(framebuffer, 5, framebuffer.getHeight() - 3 * settings.interactive.infoFontSize, tfm::format("Pos: (%.2f, %.2f, %.2f)", scene.camera.position.x, scene.camera.position.y, scene.camera.position.z), Color(255, 255, 255, 200));
		infoFont.drawText(framebuffer, 5, framebuffer.getHeight() - 4 * settings.interactive.infoFontSize - 2, tfm::format("Rot: (%.2f, %.2f, %.2f)", scene.camera.orientation.yaw, scene.camera.orientation.pitch, scene.camera.orientation.roll), Color(255, 255, 255, 200));
		infoFont.drawText(framebuffer, 5, framebuffer.getHeight() - 5 * settings.interactive.infoFontSize - 4, tfm::format("Rays: %d", rayCount.load()), Color(255, 255, 255, 200));
	}
}

void CpuTracingState::windowResized(int width, int height)
{
	(void)width;
	(void)height;
}

void CpuTracingState::framebufferResized(int width, int height)
{
	scene.camera.setImagePlaneSize(width, height);
}
