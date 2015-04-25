// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "InteractiveStates/RaytracingState.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Runners/InteractiveRunner.h"
#include "Rendering/Framebuffer.h"
#include "Raytracing/Raytracer.h"
#include "CLRaytracing/CLRaytracer.h"

using namespace Raycer;

void RaytracingState::initialize()
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	CLRaytracer& clRaytracer = App::getCLRaytracer();

	//scene.loadFromFile(settings.scene.fileName);
	scene = Scene::createTestScene();
	scene.initialize();
	scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());

	if (settings.openCL.enabled)
		clRaytracer.initialize();
}

void RaytracingState::pause()
{
}

void RaytracingState::resume()
{
}

void RaytracingState::shutdown()
{
}

void RaytracingState::update(double timeStep)
{
	InteractiveRunner& runner = App::getInteractiveRunner();

	scene.camera.update(timeStep);
	scene.camera.precalculate();

	if (runner.keyWasPressed(GLFW_KEY_F7))
		scene.saveAs("scene.json");

	if (runner.keyWasPressed(GLFW_KEY_F8))
	{
		runner.pause();
		scene.saveAs("temp.json");
#ifdef _WIN32
		ShellExecuteA(NULL, "open", "raycer.exe", "-s temp.json --non-interactive --view", NULL, SW_SHOWNORMAL);
#endif
	}
}

void RaytracingState::render(double timeStep, double interpolation)
{
	(void)timeStep;
	(void)interpolation;

	Framebuffer& framebuffer = App::getFramebuffer();
	Settings& settings = App::getSettings();
	Raytracer& raytracer = App::getRaytracer();
	CLRaytracer& clRaytracer = App::getCLRaytracer();
	InteractiveRunner& runner = App::getInteractiveRunner();
	Text& text = runner.getDefaultText();

	state.renderTarget = &framebuffer;
	state.scene = &scene;
	state.sceneWidth = framebuffer.getWidth();
	state.sceneHeight = framebuffer.getHeight();
	state.pixelOffset = 0;
	state.pixelCount = state.sceneWidth * state.sceneHeight;
	state.pixelsProcessed = 0;
	state.raysProcessed = 0;

	if (!settings.openCL.enabled)
		raytracer.run(state, interrupted);
	else
		clRaytracer.run(state, interrupted);

	if (settings.window.showCameraInfo)
	{
		text.drawText(5.0, (double)(runner.getWindowHeight() - 3 * settings.window.defaultFontSize), Color(255, 255, 255, 255), tfm::format("Pos: (%.2f, %.2f, %.2f)", scene.camera.position.x, scene.camera.position.y, scene.camera.position.z));
		text.drawText(5.0, (double)(runner.getWindowHeight() - 4 * settings.window.defaultFontSize - 2), Color(255, 255, 255, 255), tfm::format("Rot: (%.2f, %.2f, %.2f)", scene.camera.orientation.yaw, scene.camera.orientation.pitch, scene.camera.orientation.roll));
		text.drawText(5.0, (double)(runner.getWindowHeight() - 5 * settings.window.defaultFontSize - 4), Color(255, 255, 255, 255), tfm::format("Rays: %d", state.raysProcessed.load()));
	}
}

void RaytracingState::windowResized(int width, int height)
{
	(void)width;
	(void)height;
}

void RaytracingState::framebufferResized(int width, int height)
{
	scene.camera.setImagePlaneSize(width, height);
}
