// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "glfw/glfw3.h"

#include "Runners/InteractiveRunner.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Rendering/Framebuffer.h"
#include "GpuRaytracing/OpenCL.h"
#include "GpuRaytracing/GpuRaytracer.h"
#include "Rendering/Image.h"
#include "States/CpuTracingState.h"
#include "States/GpuTracingState.h"
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

GLFWwindow* InteractiveRunner::getGlfwWindow() const
{
	return glfwWindow;
}

size_t InteractiveRunner::getWindowWidth() const
{
	return windowWidth;
}

size_t InteractiveRunner::getWindowHeight() const
{
	return windowHeight;
}

const MouseInfo& InteractiveRunner::getMouseInfo() const
{
	return mouseInfo;
}

Font& InteractiveRunner::getInfoFont()
{
	return infoFont;
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
	OpenCL& openCl = App::getOpenCL();

	log.logInfo("Initializing GLFW library");

	glfwSetErrorCallback(::glfwErrorCallback);

	if (!glfwInit())
		throw std::runtime_error("Could not initialize GLFW library");

	glfwInitialized = true;

	log.logInfo("Creating window (%sx%s, fullscreen: %s)", settings.window.width, settings.window.height, settings.window.fullscreen);

	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glfwWindow = glfwCreateWindow((int)settings.window.width, (int)settings.window.height, "Raycer", settings.window.fullscreen ? glfwGetPrimaryMonitor() : 0, 0);

	if (!glfwWindow)
		throw std::runtime_error("Could not create the window");

	const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(glfwWindow, (videoMode->width / 2 - (int)settings.window.width / 2), (videoMode->height / 2 - (int)settings.window.height / 2));
	glfwShowWindow(glfwWindow);
	glfwMakeContextCurrent(glfwWindow);
	glfwSwapInterval(settings.window.vsync ? 1 : 0);

	if (settings.window.hideCursor)
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	framebuffer.initialize();
	framebuffer.enableSmoothing(settings.framebuffer.smoothing);
	
	if (settings.openCl.enabled)
	{
		openCl.initialize();
		openCl.loadKernels();
	}

	windowResized(settings.window.width, settings.window.height);

	infoFont.load(settings.window.infoFont, settings.window.infoFontSize);

	runnerStates[RunnerStates::CpuTracing] = std::make_unique<CpuTracingState>();
	runnerStates[RunnerStates::GpuTracing] = std::make_unique<GpuTracingState>();

	changeState(settings.openCl.enabled ? RunnerStates::GpuTracing : RunnerStates::CpuTracing);
}

void InteractiveRunner::shutdown()
{
	runnerStates[currentState]->shutdown();

	Font::closeFreeType();

	if (glfwInitialized)
		glfwTerminate();
}

void InteractiveRunner::windowResized(size_t width, size_t height)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	OpenCL& openCl = App::getOpenCL();
	GpuRaytracer& gpuRaytracer = App::getGpuRaytracer();

	windowWidth = width;
	windowHeight = height;

	glViewport(0, 0, (GLsizei)windowWidth, (GLsizei)windowHeight);

	if (settings.openCl.enabled)
		openCl.releaseMemoryObjects();

	framebuffer.setSize((size_t)((double)windowWidth * settings.framebuffer.scale + 0.5), (size_t)((double)windowHeight * settings.framebuffer.scale + 0.5));
	
	if (settings.openCl.enabled)
	{
		openCl.setSize(framebuffer.getWidth(), framebuffer.getHeight());
		gpuRaytracer.setSize(framebuffer.getWidth(), framebuffer.getHeight());
	}

	if (currentState != RunnerStates::None)
	{
		runnerStates[currentState]->windowResized(windowWidth, windowHeight);
		runnerStates[currentState]->framebufferResized(framebuffer.getWidth(), framebuffer.getHeight());
	}
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

	updateFpsCounter.countFrame();
	updateFpsCounter.update(timeStep);
	renderFpsCounter.update(timeStep);

	glfwPollEvents();

	int newWindowWidthTemp, newWindowHeightTemp;
	glfwGetFramebufferSize(glfwWindow, &newWindowWidthTemp, &newWindowHeightTemp);
	size_t newWindowWidth = (size_t)newWindowWidthTemp;
	size_t newWindowHeight = (size_t)newWindowHeightTemp;

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
		size_t newWidth = (size_t)((double)windowWidth * newScale + 0.5);
		size_t newHeight = (size_t)((double)windowHeight * newScale + 0.5);

		if (newWidth >= 2 && newHeight >= 2)
		{
			settings.framebuffer.scale = newScale;
			framebuffer.setSize(newWidth, newHeight);
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

			framebuffer.setSize((size_t)((double)windowWidth * settings.framebuffer.scale + 0.5), (size_t)((double)windowHeight * settings.framebuffer.scale + 0.5));
			runnerStates[currentState]->framebufferResized(framebuffer.getWidth(), framebuffer.getHeight());
		}
	}

	if (currentState != RunnerStates::None)
		runnerStates[currentState]->update(timeStep);
	else
		throw std::runtime_error("Runner state has not been set");
}

void InteractiveRunner::render(double timeStep, double interpolation)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();

	renderFpsCounter.countFrame();

	if (currentState != RunnerStates::None)
		runnerStates[currentState]->render(timeStep, interpolation);
	else
		throw std::runtime_error("Runner state has not been set");

	if (settings.window.showFps)
		infoFont.drawText(framebuffer, 5, framebuffer.getHeight() - settings.window.infoFontSize - 2, renderFpsCounter.getFpsString(), Color(255, 255, 255, 200));

	if (keyWasPressed(GLFW_KEY_F8))
	{
		Image image = Image(framebuffer);
		image.saveAs("screenshot.png");
	}

	framebuffer.render();
	framebuffer.clear();

	glfwSwapBuffers(glfwWindow);
}
