// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "glfw/glfw3.h"

#include "App/App.h"
#include "Rendering/Font.h"
#include "States/TestState.h"
#include "Math/Color.h"

using namespace Raycer;

namespace
{
	NamedLog* staticLog = nullptr;

	void GlfwErrorCallback(int error, const char* description)
	{
		if (staticLog != nullptr)
			staticLog->logError("GLFW error (%s): %s", error, description);
	}
}

int App::run()
{
	try
	{
		initialize();
		mainLoop();
	}
	catch (...)
	{
		if (log != nullptr)
			log->logException(std::current_exception());

		return -1;
	}

	try
	{
		shutdown();
	}
	catch (...)
	{
		if (log != nullptr)
			log->logException(std::current_exception());

		return -1;
	}

	return 0;
}

void App::stop()
{
	shouldRun = false;
}

GLFWwindow* App::getGlfwWindow() const
{
	return glfwWindow;
}

int App::getWindowWidth() const
{
	return windowWidth;
}

int App::getWindowHeight() const
{
	return windowHeight;
}

BaseLog& App::getBaseLog() const
{
	return *baseLog;
}

const MouseInfo& App::getMouseInfo() const
{
	return mouseInfo;
}

bool App::keyIsDown(int key)
{
	return (glfwGetKey(glfwWindow, key) == GLFW_PRESS);
}

bool App::mouseIsDown(int button)
{
	return (glfwGetMouseButton(glfwWindow, button) == GLFW_PRESS);
}

bool App::keyWasPressed(int key)
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

void App::changeState(AppStates newState)
{
	if (currentState != AppStates::None)
		appStates[currentState]->shutdown();

	currentState = newState;

	appStates[currentState]->initialize();
}

void App::initialize()
{
	baseLog = std::unique_ptr<BaseLog>(new BaseLog("raycer.log"));
	log = baseLog->getNamedLog("App");
	staticLog = log.get();

	log->logInfo("Initializing");

	iniReader = std::unique_ptr<IniReader>(new IniReader(*baseLog));
	iniReader->readFile("settings.ini");

	settings = std::unique_ptr<Settings>(new Settings(*iniReader));

	baseLog->setMinimumMessageLevel((BaseLog::MessageLevel)settings->logger.minimumMessageLevel);
	baseLog->setOutputToConsole(settings->logger.outputToConsole);

	log->logInfo("Initializing GLFW");

	glfwSetErrorCallback(::GlfwErrorCallback);

	if (!glfwInit())
		throw std::runtime_error("Could not initialize GLFW");

	glfwInitialized = true;

	log->logInfo("Creating a window (%sx%s, fullscreen: %s)", settings->window.width, settings->window.height, settings->window.enableFullscreen);

	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glfwWindow = glfwCreateWindow(settings->window.width, settings->window.height, "Raycer", settings->window.enableFullscreen ? glfwGetPrimaryMonitor() : 0, 0);

	if (!glfwWindow)
		throw std::runtime_error("Could not create the window");

	const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(glfwWindow, (videoMode->width / 2 - settings->window.width / 2), (videoMode->height / 2 - settings->window.height / 2));
	glfwShowWindow(glfwWindow);
	glfwMakeContextCurrent(glfwWindow);
	glfwSwapInterval(settings->window.enableVsync ? 1 : 0);

	if (settings->window.hideCursor)
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	framebuffer = std::unique_ptr<Framebuffer>(new Framebuffer(*baseLog));
	framebuffer->enableSmoothing(settings->framebuffer.enableSmoothing);
	
	windowResized(settings->window.width, settings->window.height);

	fpsFont = std::unique_ptr<Font>(new Font(*baseLog, settings->app.fpsFont, settings->app.fpsFontSize));

	appStates[AppStates::Test] = std::unique_ptr<TestState>(new TestState(*baseLog, *this, *framebuffer, *settings));

	changeState(AppStates::Test);
}

void App::shutdown()
{
	log->logInfo("Shutting down");

	appStates[currentState]->shutdown();

	Font::closeFreeType();

	if (glfwInitialized)
		glfwTerminate();
}

void App::windowResized(int width, int height)
{
	windowWidth = width;
	windowHeight = height;

	glViewport(0, 0, windowWidth, windowHeight);

	if (settings->framebuffer.enableResizing)
		framebuffer->setSize((int)(windowWidth * settings->framebuffer.resizeScale + 0.5), (int)(windowHeight * settings->framebuffer.resizeScale + 0.5));
	else if (framebuffer->getWidth() != settings->framebuffer.fixedWidth || framebuffer->getHeight() != settings->framebuffer.fixedHeight)
		framebuffer->setSize(settings->framebuffer.fixedWidth, settings->framebuffer.fixedHeight);

	if (currentState != AppStates::None)
	{
		appStates[currentState]->windowResized(windowWidth, windowHeight);
		appStates[currentState]->framebufferResized(framebuffer->getWidth(), framebuffer->getHeight());
	}
}

// http://gafferongames.com/game-physics/fix-your-timestep/
// http://gamesfromwithin.com/casey-and-the-clearly-deterministic-contraptions
// https://randomascii.wordpress.com/2012/02/13/dont-store-that-in-a-float/
void App::mainLoop()
{
	log->logInfo("Entering the main loop");

	double timeStep = 1.0 / settings->app.updateFrequency;
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

void App::update(double timeStep)
{
	updateFpsCounter.countFrame();
	updateFpsCounter.update(timeStep);
	renderFpsCounter.update(timeStep);

	glfwPollEvents();

	int newWindowWidth, newWindowHeight;
	glfwGetFramebufferSize(glfwWindow, &newWindowWidth, &newWindowHeight);

	if (newWindowWidth != windowWidth || newWindowHeight != windowHeight)
		windowResized(newWindowWidth, newWindowHeight);

	double newMouseX, newMouseY;
	glfwGetCursorPos(glfwWindow, &newMouseX, &newMouseY);

	mouseInfo.windowX = (int)newMouseX;
	mouseInfo.windowY = windowHeight - (int)newMouseY - 1;
	mouseInfo.framebufferX = (int)((double)mouseInfo.windowX / windowWidth * framebuffer->getWidth());
	mouseInfo.framebufferY = (int)((double)mouseInfo.windowY / windowHeight * framebuffer->getHeight());
	mouseInfo.deltaX = mouseInfo.windowX - previousMouseX;
	mouseInfo.deltaY = mouseInfo.windowY - previousMouseY;
	previousMouseX = mouseInfo.windowX;
	previousMouseY = mouseInfo.windowY;

	if (glfwWindowShouldClose(glfwWindow))
		shouldRun = false;

	if (keyWasPressed(GLFW_KEY_ESCAPE))
		shouldRun = false;

	if (keyWasPressed(GLFW_KEY_F11) && settings->framebuffer.enableResizing)
	{
		if (settings->framebuffer.resizeScale < 1.0)
		{
			settings->framebuffer.resizeScale *= 2.0;

			if (settings->framebuffer.resizeScale > 1.0)
				settings->framebuffer.resizeScale = 1.0;

			framebuffer->setSize((int)(windowWidth * settings->framebuffer.resizeScale + 0.5), (int)(windowHeight * settings->framebuffer.resizeScale + 0.5));
			appStates[currentState]->framebufferResized(framebuffer->getWidth(), framebuffer->getHeight());
		}
	}

	if (keyWasPressed(GLFW_KEY_F10) && settings->framebuffer.enableResizing)
	{
		double newScale = settings->framebuffer.resizeScale * 0.5;
		int newWidth = (int)(windowWidth * newScale + 0.5);
		int newHeight = (int)(windowHeight * newScale + 0.5);

		if (newWidth >= 2 && newHeight >= 2)
		{
			settings->framebuffer.resizeScale = newScale;
			framebuffer->setSize(newWidth, newHeight);
			appStates[currentState]->framebufferResized(framebuffer->getWidth(), framebuffer->getHeight());
		}
	}

	if (keyWasPressed(GLFW_KEY_F9))
	{
		settings->framebuffer.enableSmoothing = !settings->framebuffer.enableSmoothing;
		framebuffer->enableSmoothing(settings->framebuffer.enableSmoothing);
	}

	if (currentState != AppStates::None)
		appStates[currentState]->update(timeStep);
	else
		throw std::runtime_error("App state has not been set");
}

void App::render(double timeStep, double interpolation)
{
	renderFpsCounter.countFrame();

	if (currentState != AppStates::None)
		appStates[currentState]->render(timeStep, interpolation);
	else
		throw std::runtime_error("App state has not been set");

	if (settings->app.showFps)
		fpsFont->drawText(*framebuffer, 5, framebuffer->getHeight() - settings->app.fpsFontSize - 2, renderFpsCounter.getFpsString(), Color(255, 255, 255, 200));

	framebuffer->render();
	framebuffer->clear();

	glfwSwapBuffers(glfwWindow);
}
