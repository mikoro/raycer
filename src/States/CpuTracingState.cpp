// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

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
	InteractiveRunner& runner = App::getInteractiveRunner();
	Text& text = runner.getDefaultText();

	info.renderTarget = &framebuffer;
	info.scene = &scene;
	info.sceneWidth = framebuffer.getWidth();
	info.sceneHeight = framebuffer.getHeight();
	info.pixelStartOffset = 0;
	info.pixelTotalCount = info.sceneWidth * info.sceneHeight;
	info.pixelsProcessed = 0;
	info.raysProcessed = 0;

	cpuRaytracer.trace(info, interrupted);

	if (settings.window.showCameraInfo)
	{
		text.drawText(5.0, (double)(runner.getWindowHeight() - 3 * settings.window.defaultFontSize), Color(255, 255, 255, 255), tfm::format("Pos: (%.2f, %.2f, %.2f)", scene.camera.position.x, scene.camera.position.y, scene.camera.position.z));
		text.drawText(5.0, (double)(runner.getWindowHeight() - 4 * settings.window.defaultFontSize - 2), Color(255, 255, 255, 255), tfm::format("Rot: (%.2f, %.2f, %.2f)", scene.camera.orientation.yaw, scene.camera.orientation.pitch, scene.camera.orientation.roll));
		text.drawText(5.0, (double)(runner.getWindowHeight() - 5 * settings.window.defaultFontSize - 4), Color(255, 255, 255, 255), tfm::format("Rays: %d", info.raysProcessed.load()));
	}
}

void CpuTracingState::windowResized(size_t width, size_t height)
{
	(void)width;
	(void)height;
}

void CpuTracingState::framebufferResized(size_t width, size_t height)
{
	scene.camera.setImagePlaneSize(width, height);
}
