// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#ifdef _WIN32
#include <windows.h>
#endif

#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Runners/InteractiveRunner.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Rendering/Framebuffer.h"
#include "Rendering/GLHelper.h"
#include "CLRaytracing/CLManager.h"
#include "CLRaytracing/CLRaytracer.h"
#include "Rendering/Image.h"
#include "InteractiveStates/RaytracingState.h"
#include "Math/Color.h"

using namespace Raycer;

namespace
{
	void glfwErrorCallback(int error, const char* description)
	{
		App::getLog().logError("GLFW error (%s): %s", error, description);
	}
}

InteractiveRunner::InteractiveRunner()
{
}

int InteractiveRunner::run()
{
	try
	{
		initialize();
		mainLoop();
	}
	catch (...)
	{
		App::getLog().logException(std::current_exception());
		return -1;
	}

	try
	{
		shutdown();
	}
	catch (...)
	{
		App::getLog().logException(std::current_exception());
		return -1;
	}

	return 0;
}

void InteractiveRunner::stop()
{
	shouldRun = false;
}

void InteractiveRunner::pause()
{
	isPaused = true;
}

GLFWwindow* InteractiveRunner::getGlfwWindow() const
{
	return glfwWindow;
}

int InteractiveRunner::getWindowWidth() const
{
	return windowWidth;
}

int InteractiveRunner::getWindowHeight() const
{
	return windowHeight;
}

const MouseInfo& InteractiveRunner::getMouseInfo() const
{
	return mouseInfo;
}

Text& InteractiveRunner::getDefaultText()
{
	return defaultText;
}

double InteractiveRunner::getElapsedTime() const
{
	return glfwGetTime() - startTime;
}

bool InteractiveRunner::keyIsDown(int key)
{
	return (glfwGetKey(glfwWindow, key) == GLFW_PRESS);
}

bool InteractiveRunner::mouseIsDown(int button)
{
	return (glfwGetMouseButton(glfwWindow, button) == GLFW_PRESS);
}

bool InteractiveRunner::keyWasPressed(int key)
{
	if (keyIsDown(key))
	{
		if (!keyStates[key])
		{
			keyStates[key] = true;
			return true;
		}
	}
	else
		keyStates[key] = false;

	return false;
}

void InteractiveRunner::changeState(RunnerStates newState)
{
	if (currentState != RunnerStates::None)
		runnerStates[currentState]->shutdown();

	currentState = newState;

	runnerStates[currentState]->initialize();
}

void InteractiveRunner::initialize()
{
	Log& log = App::getLog();
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	CLManager& clManager = App::getCLManager();

	log.logInfo("Initializing GLFW library");

	glfwSetErrorCallback(::glfwErrorCallback);

	if (!glfwInit())
		throw std::runtime_error("Could not initialize GLFW library");

	glfwInitialized = true;
	startTime = glfwGetTime();

	log.logInfo("Creating window (%sx%s, fullscreen: %s)", settings.window.width, settings.window.height, settings.window.fullscreen);

	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glfwWindow = glfwCreateWindow(settings.window.width, settings.window.height, "Raycer", settings.window.fullscreen ? glfwGetPrimaryMonitor() : 0, 0);

	if (!glfwWindow)
		throw std::runtime_error("Could not create the window");

	const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(glfwWindow, (videoMode->width / 2 - settings.window.width / 2), (videoMode->height / 2 - settings.window.height / 2));
	glfwShowWindow(glfwWindow);
	glfwMakeContextCurrent(glfwWindow);

	log.logInfo("Initializing GLEW library");

	GLenum result = glewInit();

	if (result != GLEW_OK)
		throw std::runtime_error(tfm::format("Could not initialize GLEW library: %s", glewGetErrorString(result)));

	glfwSwapInterval(settings.window.vsync ? 1 : 0);

	if (settings.window.hideCursor)
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	framebuffer.initialize();
	framebuffer.enableSmoothing(settings.framebuffer.smoothing);
	
	if (settings.openCL.enabled)
	{
		clManager.initialize();
		clManager.loadKernels();
	}

	defaultText.initialize(settings.window.defaultFont, settings.window.defaultFontSize);
	pauseText.initialize(settings.window.defaultFont, 100);

	windowResized(settings.window.width, settings.window.height);

	runnerStates[RunnerStates::Raytracing] = std::unique_ptr<RaytracingState>(new RaytracingState());

	changeState(RunnerStates::Raytracing);
}

void InteractiveRunner::shutdown()
{
	runnerStates[currentState]->shutdown();

	if (glfwInitialized)
		glfwTerminate();
}

void InteractiveRunner::windowResized(int width, int height)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	
	windowWidth = width;
	windowHeight = height;

	glViewport(0, 0, (GLsizei)windowWidth, (GLsizei)windowHeight);
	defaultText.setWindowSize(windowWidth, windowHeight);
	pauseText.setWindowSize(windowWidth, windowHeight);

	resizeFramebuffer((int)((double)windowWidth * settings.framebuffer.scale + 0.5), (int)((double)windowHeight * settings.framebuffer.scale + 0.5));

	if (currentState != RunnerStates::None)
	{
		runnerStates[currentState]->windowResized(windowWidth, windowHeight);
		runnerStates[currentState]->framebufferResized(framebuffer.getWidth(), framebuffer.getHeight());
	}
}

void InteractiveRunner::resizeFramebuffer(int width, int height)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	CLRaytracer& clRaytracer = App::getCLRaytracer();

	if (settings.openCL.enabled)
		clRaytracer.releasePixelBuffer();

	framebuffer.resize(width, height);

	if (settings.openCL.enabled)
		clRaytracer.resizePixelBuffer(framebuffer.getWidth(), framebuffer.getHeight());
}

// http://gafferongames.com/game-physics/fix-your-timestep/
// http://gamesfromwithin.com/casey-and-the-clearly-deterministic-contraptions
// https://randomascii.wordpress.com/2012/02/13/dont-store-that-in-a-float/
void InteractiveRunner::mainLoop()
{
	App::getLog().logInfo("Entering the main loop");

	double timeStep = 1.0 / 60.0;
	double previousTime = glfwGetTime();
	double timeAccumulator = 0;

	update(0.0);

	while (shouldRun)
	{
		double currentTime = glfwGetTime();
		double frameTime = currentTime - previousTime;
		previousTime = currentTime;

		// prevent too large frametimes (e.g. program was paused)
		if (frameTime > 0.25)
			frameTime = 0.25;

		timeAccumulator += frameTime;

		while (timeAccumulator >= timeStep)
		{
			update(timeStep);
			timeAccumulator -= timeStep;
		}

		double interpolation = timeAccumulator / timeStep;
		render(frameTime, interpolation);
	}
}

void InteractiveRunner::update(double timeStep)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();

	updateFpsCounter.count();
	updateFpsCounter.update(timeStep);
	renderFpsCounter.update(timeStep);

	glfwPollEvents();

	int newWindowWidth, newWindowHeight;
	glfwGetFramebufferSize(glfwWindow, &newWindowWidth, &newWindowHeight);

	if (newWindowWidth != windowWidth || newWindowHeight != windowHeight)
		windowResized(newWindowWidth, newWindowHeight);

	double newMouseX, newMouseY;
	glfwGetCursorPos(glfwWindow, &newMouseX, &newMouseY);

	mouseInfo.windowX = (int)(newMouseX + 0.5);
	mouseInfo.windowY = (int)((double)windowHeight - newMouseY - 1.0 + 0.5);
	mouseInfo.framebufferX = (int)((double)mouseInfo.windowX / (double)(windowWidth * framebuffer.getWidth()) + 0.5);
	mouseInfo.framebufferY = (int)((double)mouseInfo.windowY / (double)(windowHeight * framebuffer.getHeight()) + 0.5);
	mouseInfo.deltaX = mouseInfo.windowX - previousMouseX;
	mouseInfo.deltaY = mouseInfo.windowY - previousMouseY;
	previousMouseX = mouseInfo.windowX;
	previousMouseY = mouseInfo.windowY;

	if (glfwWindowShouldClose(glfwWindow))
		shouldRun = false;

	if (keyWasPressed(GLFW_KEY_ESCAPE))
		shouldRun = false;

	if (keyWasPressed(GLFW_KEY_F1))
		settings.window.showFps = !settings.window.showFps;

	if (keyWasPressed(GLFW_KEY_F2))
		settings.window.showCameraInfo = !settings.window.showCameraInfo;

	if (keyWasPressed(GLFW_KEY_F9))
	{
		settings.framebuffer.smoothing = !settings.framebuffer.smoothing;
		framebuffer.enableSmoothing(settings.framebuffer.smoothing);
	}

	if (keyWasPressed(GLFW_KEY_F10))
	{
		double newScale = settings.framebuffer.scale * 0.5;
		int newWidth = (int)((double)windowWidth * newScale + 0.5);
		int newHeight = (int)((double)windowHeight * newScale + 0.5);

		if (newWidth >= 2 && newHeight >= 2)
		{
			settings.framebuffer.scale = newScale;
			resizeFramebuffer(newWidth, newHeight);
			runnerStates[currentState]->framebufferResized(framebuffer.getWidth(), framebuffer.getHeight());
		}
	}

	if (keyWasPressed(GLFW_KEY_F11))
	{
		if (settings.framebuffer.scale < 1.0)
		{
			settings.framebuffer.scale *= 2.0;

			if (settings.framebuffer.scale > 1.0)
				settings.framebuffer.scale = 1.0;

			resizeFramebuffer((int)((double)windowWidth * settings.framebuffer.scale + 0.5), (int)((double)windowHeight * settings.framebuffer.scale + 0.5));
			runnerStates[currentState]->framebufferResized(framebuffer.getWidth(), framebuffer.getHeight());
		}
	}

	if (keyWasPressed(GLFW_KEY_P))
		isPaused = !isPaused;

	if (currentState != RunnerStates::None)
	{
		if (!isPaused)
			runnerStates[currentState]->update(timeStep);
	}
	else
		throw std::runtime_error("Runner state has not been set");
}

void InteractiveRunner::render(double timeStep, double interpolation)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();

	renderFpsCounter.count();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//framebuffer.clear(); // not needed if every pixel is written every time

	if (currentState != RunnerStates::None)
	{
		if (!isPaused)
			runnerStates[currentState]->render(timeStep, interpolation);
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	else
		throw std::runtime_error("Runner state has not been set");

	framebuffer.render();
	
	if (settings.window.showFps)
		defaultText.drawText(5.0, (double)(windowHeight - settings.window.defaultFontSize - 2), Color::WHITE, renderFpsCounter.getFpsString());

	defaultText.render();

	if (isPaused)
	{
		pauseText.drawText((double)windowWidth / 2.0 - 200.0, (double)windowHeight / 2.0 - 40.0, Color::WHITE, "PAUSED");
		pauseText.render();
	}
	
	glfwSwapBuffers(glfwWindow);

	if (keyWasPressed(GLFW_KEY_F12))
		takeScreenshot();
}

void InteractiveRunner::takeScreenshot() const
{
	std::vector<float> data(windowWidth * windowHeight * 4);

	glReadPixels(0, 0, (GLsizei)windowWidth, (GLsizei)windowHeight, GL_RGBA, GL_FLOAT, &data[0]);
	GLHelper::checkError("Could not read pixels from renderbuffer");

	Image image(windowWidth, windowHeight, &data[0]);
	image.saveAs("screenshot.png");
}
