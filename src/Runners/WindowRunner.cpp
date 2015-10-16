// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Runners/WindowRunner.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Rendering/Framebuffer.h"
#include "Utils/GLHelper.h"
#include "OpenCL/CLManager.h"
#include "OpenCL/CLTracer.h"
#include "Rendering/Image.h"
#include "Runners/WindowRunnerStates/WindowRunnerState.h"
#include "Runners/WindowRunnerStates/DefaultState.h"
#include "Math/Color.h"

using namespace Raycer;

namespace
{
	void glfwErrorCallback(int error, const char* description)
	{
		App::getLog().logError("GLFW error (%s): %s", error, description);
	}

	MouseInfo* mouseInfoPtr = nullptr;

	void glfwMouseWheelScroll(GLFWwindow* window, double xoffset, double yoffset)
	{
		(void)window;
		(void)xoffset;

		mouseInfoPtr->scrollY = yoffset;
		mouseInfoPtr->hasScrolled = true;
	}
}

WindowRunner::WindowRunner()
{
}

WindowRunner::~WindowRunner()
{
	if (glfwInitialized)
		glfwTerminate();
}

int WindowRunner::run()
{
	initialize();
	mainLoop();
	shutdown();

	return 0;
}

void WindowRunner::stop()
{
	shouldRun = false;
}

void WindowRunner::pause()
{
	isPaused = true;
}

GLFWwindow* WindowRunner::getGlfwWindow() const
{
	return glfwWindow;
}

size_t WindowRunner::getWindowWidth() const
{
	return windowWidth;
}

size_t WindowRunner::getWindowHeight() const
{
	return windowHeight;
}

const MouseInfo& WindowRunner::getMouseInfo() const
{
	return mouseInfo;
}

Text& WindowRunner::getDefaultText()
{
	return defaultText;
}

double WindowRunner::getElapsedTime() const
{
	return glfwGetTime() - startTime;
}

double WindowRunner::getFps() const
{
	return renderFpsCounter.getFps();
}

bool WindowRunner::keyIsDown(int key)
{
	return (glfwGetKey(glfwWindow, key) == GLFW_PRESS);
}

bool WindowRunner::mouseIsDown(int button)
{
	return (glfwGetMouseButton(glfwWindow, button) == GLFW_PRESS);
}

bool WindowRunner::keyWasPressed(int key)
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

bool WindowRunner::mouseWasPressed(int button)
{
	if (mouseIsDown(button))
	{
		if (!mouseStates[button])
		{
			mouseStates[button] = true;
			return true;
		}
	}
	else
		mouseStates[button] = false;

	return false;
}

double WindowRunner::getMouseWheelScroll()
{
	if (mouseInfo.hasScrolled)
	{
		mouseInfo.hasScrolled = false;
		return mouseInfo.scrollY;
	}

	return 0.0;
}

void WindowRunner::changeState(WindowRunnerStates newState)
{
	if (currentState != WindowRunnerStates::None)
		runnerStates[currentState]->shutdown();

	currentState = newState;

	runnerStates[currentState]->initialize();
}

void WindowRunner::initialize()
{
	Log& log = App::getLog();
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	CLManager& clManager = App::getCLManager();

	mouseInfoPtr = &mouseInfo;

	log.logInfo("Initializing GLFW library");

	glfwSetErrorCallback(::glfwErrorCallback);

	if (!glfwInit())
		throw std::runtime_error("Could not initialize GLFW library");

	glfwInitialized = true;
	startTime = glfwGetTime();

	log.logInfo("Creating window and OpenGL context (%sx%s, fullscreen: %s)", settings.window.width, settings.window.height, settings.window.enableFullscreen);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindow = glfwCreateWindow(int(settings.window.width), int(settings.window.height), "Raycer", settings.window.enableFullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

	if (!glfwWindow)
		throw std::runtime_error("Could not create the window");

	glfwSetScrollCallback(glfwWindow, ::glfwMouseWheelScroll);

	const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(glfwWindow, (videoMode->width / 2 - int(settings.window.width) / 2), (videoMode->height / 2 - int(settings.window.height) / 2));
	glfwMakeContextCurrent(glfwWindow);

	log.logInfo("Initializing GL3W library");

	int result = gl3wInit();

	if (result == -1)
		throw std::runtime_error("Could not initialize GL3W library");

	log.logInfo("OpenGL info: %s/%s | Version: %s | GLSL: %s", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

	glfwSwapInterval(settings.window.enableVsync ? 1 : 0);

	if (settings.window.hideCursor)
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	framebuffer.initialize();
	framebuffer.enableSmoothing(settings.framebuffer.enableSmoothing);

	if (settings.openCL.enabled)
		clManager.initialize();

	defaultText.initialize(settings.window.defaultFont, double(settings.window.defaultFontSize));
	pauseText.initialize(settings.window.defaultFont, 100);

	windowResized(settings.window.width, settings.window.height);

	runnerStates[WindowRunnerStates::Default] = std::make_shared<DefaultState>();
	changeState(WindowRunnerStates::Default);
}

void WindowRunner::shutdown()
{
	runnerStates[currentState]->shutdown();
}

void WindowRunner::windowResized(size_t width, size_t height)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();

	if (width == 0 || height == 0)
		return;

	windowWidth = width;
	windowHeight = height;

	glViewport(0, 0, GLsizei(windowWidth), GLsizei(windowHeight));
	defaultText.setWindowSize(windowWidth, windowHeight);
	pauseText.setWindowSize(windowWidth, windowHeight);

	size_t framebufferImageWidth = size_t(double(windowWidth) * settings.framebuffer.scale + 0.5);
	size_t framebufferImageHeight = size_t(double(windowHeight) * settings.framebuffer.scale + 0.5);
	resizeFramebuffer(framebufferImageWidth, framebufferImageHeight);
	framebuffer.setWindowSize(windowWidth, windowHeight);

	if (currentState != WindowRunnerStates::None)
	{
		runnerStates[currentState]->windowResized(windowWidth, windowHeight);
		runnerStates[currentState]->framebufferResized(framebufferImageWidth, framebufferImageHeight);
	}
}

void WindowRunner::resizeFramebuffer(size_t width, size_t height)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	CLTracer& clTracer = App::getCLTracer();

	if (settings.openCL.enabled)
		clTracer.releaseImageBuffer();

	framebuffer.resize(width, height);

	if (settings.openCL.enabled)
		clTracer.initializeImageBuffer(width, height);
}

// http://gafferongames.com/game-physics/fix-your-timestep/
// http://gamesfromwithin.com/casey-and-the-clearly-deterministic-contraptions
// https://randomascii.wordpress.com/2012/02/13/dont-store-that-in-a-float/
void WindowRunner::mainLoop()
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

void WindowRunner::update(double timeStep)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();

	updateFpsCounter.count();
	updateFpsCounter.update(timeStep);
	renderFpsCounter.update(timeStep);

	glfwPollEvents();

	int newWindowWidth, newWindowHeight;
	glfwGetFramebufferSize(glfwWindow, &newWindowWidth, &newWindowHeight);

	if (size_t(newWindowWidth) != windowWidth || size_t(newWindowHeight) != windowHeight)
		windowResized(size_t(newWindowWidth), size_t(newWindowHeight));

	double newMouseX, newMouseY;
	glfwGetCursorPos(glfwWindow, &newMouseX, &newMouseY);

	mouseInfo.windowX = int(newMouseX + 0.5);
	mouseInfo.windowY = int(double(windowHeight) - newMouseY - 1.0 + 0.5);
	mouseInfo.framebufferX = int((mouseInfo.windowX / double(windowWidth)) * framebuffer.getWidth() + 0.5);
	mouseInfo.framebufferY = int((mouseInfo.windowY / double(windowHeight)) * framebuffer.getHeight() + 0.5);
	mouseInfo.deltaX = mouseInfo.windowX - previousMouseX;
	mouseInfo.deltaY = mouseInfo.windowY - previousMouseY;
	previousMouseX = mouseInfo.windowX;
	previousMouseY = mouseInfo.windowY;

	if (glfwWindowShouldClose(glfwWindow))
		shouldRun = false;

	if (keyWasPressed(GLFW_KEY_ESCAPE))
		shouldRun = false;

	if (keyWasPressed(GLFW_KEY_F1))
		settings.window.showInfoText = !settings.window.showInfoText;

	if (keyWasPressed(GLFW_KEY_F9))
	{
		settings.framebuffer.enableSmoothing = !settings.framebuffer.enableSmoothing;
		framebuffer.enableSmoothing(settings.framebuffer.enableSmoothing);
	}

	if (keyWasPressed(GLFW_KEY_F10))
	{
		double newScale = settings.framebuffer.scale * 0.5;
		size_t newWidth = size_t(double(windowWidth) * newScale + 0.5);
		size_t newHeight = size_t(double(windowHeight) * newScale + 0.5);

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

			resizeFramebuffer(size_t(double(windowWidth) * settings.framebuffer.scale + 0.5), size_t(double(windowHeight) * settings.framebuffer.scale + 0.5));
			runnerStates[currentState]->framebufferResized(framebuffer.getWidth(), framebuffer.getHeight());
		}
	}

	if (keyWasPressed(GLFW_KEY_P))
		isPaused = !isPaused;

	if (currentState != WindowRunnerStates::None)
	{
		if (!isPaused)
			runnerStates[currentState]->update(timeStep);
	}
	else
		throw std::runtime_error("Runner state has not been set");
}

void WindowRunner::render(double timeStep, double interpolation)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();

	renderFpsCounter.count();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (currentState != WindowRunnerStates::None)
	{
		if (!isPaused)
			runnerStates[currentState]->render(timeStep, interpolation);
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	else
		throw std::runtime_error("Runner state has not been set");

	framebuffer.render();

	if (settings.window.showInfoText)
		defaultText.drawText(5.0, double(windowHeight - settings.window.defaultFontSize - 2), Color::WHITE, renderFpsCounter.getFpsString());

	defaultText.render();

	if (isPaused)
	{
		pauseText.drawText(double(windowWidth) / 2.0 - 200.0, double(windowHeight) / 2.0 - 40.0, Color::WHITE, "PAUSED");
		pauseText.render();
	}

	glfwSwapBuffers(glfwWindow);

	if (keyWasPressed(GLFW_KEY_F12))
		takeScreenshot();
}

void WindowRunner::takeScreenshot() const
{
	std::vector<float> data(windowWidth * windowHeight * 4);

	glReadPixels(0, 0, GLsizei(windowWidth), GLsizei(windowHeight), GL_RGBA, GL_FLOAT, &data[0]);
	GLHelper::checkError("Could not read pixels from renderbuffer");

	Image image(windowWidth, windowHeight, &data[0]);
	image.save("screenshot.png");
}
