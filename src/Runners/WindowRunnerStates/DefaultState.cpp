// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#endif

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <tinyformat/tinyformat.h>

#include "Runners/WindowRunnerStates/DefaultState.h"
#include "App.h"
#include "Utils/Log.h"
#include "OpenCL/CLRaytracer.h"
#include "Math/Color.h"
#include "Math/EulerAngle.h"
#include "Math/Vector3.h"
#include "Raytracing/Camera.h"
#include "Raytracing/Raytracer.h"
#include "Rendering/Framebuffer.h"
#include "Rendering/Text.h"
#include "Runners/WindowRunner.h"
#include "Utils/Settings.h"

using namespace Raycer;

void DefaultState::initialize()
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	CLRaytracer& clRaytracer = App::getCLRaytracer();

	if (settings.scene.enableTestScenes)
		scene = Scene::createTestScene(settings.scene.testSceneNumber);
	else
		scene = Scene::loadFromFile(settings.scene.fileName);

	scene.initialize();
	scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());

	if (settings.openCL.enabled)
		clRaytracer.initialize();

	currentTestSceneNumber = settings.scene.testSceneNumber;
}

void DefaultState::pause()
{
}

void DefaultState::resume()
{
}

void DefaultState::shutdown()
{
}

void DefaultState::update(double timeStep)
{
	WindowRunner& windowRunner = App::getWindowRunner();
	Framebuffer& framebuffer = App::getFramebuffer();

	bool increaseTestSceneNumber = windowRunner.keyWasPressed(GLFW_KEY_F2);
	bool decreaseTestSceneNumber = windowRunner.keyWasPressed(GLFW_KEY_F3);

	if (increaseTestSceneNumber || decreaseTestSceneNumber)
	{
		if (increaseTestSceneNumber)
			currentTestSceneNumber++;

		if (decreaseTestSceneNumber)
			currentTestSceneNumber--;

		scene = Scene::createTestScene(currentTestSceneNumber);
		scene.initialize();
		scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());
	}

	if (windowRunner.keyWasPressed(GLFW_KEY_F6))
		scene.raytracer.visualizeDepth = !scene.raytracer.visualizeDepth;

	if (windowRunner.keyWasPressed(GLFW_KEY_F7))
		scene.saveToFile("scene.xml");

	if (windowRunner.keyWasPressed(GLFW_KEY_F8))
	{
		windowRunner.pause();
		scene.saveToFile("scene.xml");

#ifdef _WIN32
		ShellExecuteA(NULL, "open", "raycer.exe", "-s scene.xml --non-interactive --non-test --view", NULL, SW_SHOWNORMAL);
#else
		int pid = fork();

		if (pid == 0)
		{
			char* arg[] = { (char*)"raycer", (char*)"-s scene.xml", (char*)"--non-interactive", (char*)"--non-test", (char*)"--view", (char*)0 };

			if (execvp(arg[0], arg) == -1)
				App::getLog().logWarning("Could not launch external rendering (%d) (try adding raycer to PATH)", errno);
		}
#endif
	}

	scene.camera.update(scene, timeStep);
}

void DefaultState::render(double timeStep, double interpolation)
{
	(void)timeStep;
	(void)interpolation;

	Framebuffer& framebuffer = App::getFramebuffer();
	Settings& settings = App::getSettings();
	Raytracer& raytracer = App::getRaytracer();
	CLRaytracer& clRaytracer = App::getCLRaytracer();
	WindowRunner& runner = App::getWindowRunner();
	Text& text = runner.getDefaultText();

	state.image = &framebuffer.getImage();
	state.scene = &scene;
	state.sceneWidth = framebuffer.getWidth();
	state.sceneHeight = framebuffer.getHeight();
	state.pixelOffset = 0;
	state.pixelCount = state.sceneWidth * state.sceneHeight;
	state.pixelsProcessed = 0;

	if (!settings.openCL.enabled)
		raytracer.run(state, interrupted);
	else
		clRaytracer.run(state, interrupted);

	if (settings.window.showInfoText)
	{
		text.drawText(5.0, (double)(runner.getWindowHeight() - 3 * settings.window.defaultFontSize), Color(255, 255, 255, 255), tfm::format("Pos: (%.2f, %.2f, %.2f)", scene.camera.position.x, scene.camera.position.y, scene.camera.position.z));
		text.drawText(5.0, (double)(runner.getWindowHeight() - 4 * settings.window.defaultFontSize - 2), Color(255, 255, 255, 255), tfm::format("Rot: (%.2f, %.2f, %.2f)", scene.camera.orientation.pitch, scene.camera.orientation.yaw, scene.camera.orientation.roll));
		text.drawText(5.0, (double)(runner.getWindowHeight() - 5 * settings.window.defaultFontSize - 4), Color(255, 255, 255, 255), tfm::format("Pix: (%d, %d)", runner.getMouseInfo().framebufferX, runner.getMouseInfo().framebufferY));
		text.drawText(5.0, (double)(runner.getWindowHeight() - 6 * settings.window.defaultFontSize - 6), Color(255, 255, 255, 255), tfm::format("Mov: %s", scene.camera.hasMoved()));
	}
}

void DefaultState::windowResized(int width, int height)
{
	(void)width;
	(void)height;
}

void DefaultState::framebufferResized(int width, int height)
{
	scene.camera.setImagePlaneSize(width, height);
}
