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

	scene.loadFromFile("data/scenes/test_scene.json");
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

	config.renderTarget = &framebuffer;
	config.scene = &scene;
	config.sceneWidth = framebuffer.getWidth();
	config.sceneHeight = framebuffer.getHeight();
	config.pixelOffset = 0;
	config.pixelCount = config.sceneWidth * config.sceneHeight;
	config.pixelsProcessed = 0;
	config.raysProcessed = 0;
	config.isInteractive = true;

	cpuRaytracer.trace(config, interrupted);

	if (settings.window.showCameraInfo)
	{
		text.drawText(5.0, (double)(runner.getWindowHeight() - 3 * settings.window.defaultFontSize), Color(255, 255, 255, 255), tfm::format("Pos: (%.2f, %.2f, %.2f)", scene.camera.position.x, scene.camera.position.y, scene.camera.position.z));
		text.drawText(5.0, (double)(runner.getWindowHeight() - 4 * settings.window.defaultFontSize - 2), Color(255, 255, 255, 255), tfm::format("Rot: (%.2f, %.2f, %.2f)", scene.camera.orientation.yaw, scene.camera.orientation.pitch, scene.camera.orientation.roll));
		text.drawText(5.0, (double)(runner.getWindowHeight() - 5 * settings.window.defaultFontSize - 4), Color(255, 255, 255, 255), tfm::format("Rays: %d", config.raysProcessed.load()));
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
