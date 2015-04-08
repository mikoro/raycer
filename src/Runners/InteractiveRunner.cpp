// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "glfw/glfw3.h"

#include "Runners/InteractiveRunner.h"
#include "Rendering/Image.h"
#include "States/TraceFastState.h"
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

InteractiveRunner::InteractiveRunner(BaseLog& baseLog_) : baseLog(baseLog_)
{
	log = baseLog_.getNamedLog("InteractiveRunner");
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

void InteractiveRunner::stop()
{
	shouldRun = false;
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

Font& InteractiveRunner::getInfoFont() const
{
	return *infoFont;
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
	staticLog = log.get();

	log->logInfo("Initializing interactive runner");

	iniReader = std::unique_ptr<IniReader>(new IniReader(baseLog));
	iniReader->readFile("settings.ini");

	settings = std::unique_ptr<InteractiveSettings>(new InteractiveSettings(*iniReader));

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

	framebuffer = std::unique_ptr<Framebuffer>(new Framebuffer(baseLog));
	framebuffer->enableSmoothing(settings->framebuffer.enableSmoothing);
	
	windowResized(settings->window.width, settings->window.height);

	infoFont = std::unique_ptr<Font>(new Font(baseLog, settings->runner.infoFont, settings->runner.infoFontSize));

	runnerStates[RunnerStates::TraceFast] = std::unique_ptr<TraceFastState>(new TraceFastState(baseLog, *this, *framebuffer, *settings));

	changeState(RunnerStates::TraceFast);
}

void InteractiveRunner::shutdown()
{
	log->logInfo("Shutting down interactive runner");

	runnerStates[currentState]->shutdown();

	Font::closeFreeType();

	if (glfwInitialized)
		glfwTerminate();
}

void InteractiveRunner::windowResized(int width, int height)
{
	windowWidth = width;
	windowHeight = height;

	glViewport(0, 0, windowWidth, windowHeight);

	if (settings->framebuffer.enableResizing)
		framebuffer->setSize((int)(windowWidth * settings->framebuffer.resizeScale + 0.5), (int)(windowHeight * settings->framebuffer.resizeScale + 0.5));
	else if (framebuffer->getWidth() != settings->framebuffer.fixedWidth || framebuffer->getHeight() != settings->framebuffer.fixedHeight)
		framebuffer->setSize(settings->framebuffer.fixedWidth, settings->framebuffer.fixedHeight);

	if (currentState != RunnerStates::None)
	{
		runnerStates[currentState]->windowResized(windowWidth, windowHeight);
		runnerStates[currentState]->framebufferResized(framebuffer->getWidth(), framebuffer->getHeight());
	}
}

// http://gafferongames.com/game-physics/fix-your-timestep/
// http://gamesfromwithin.com/casey-and-the-clearly-deterministic-contraptions
// https://randomascii.wordpress.com/2012/02/13/dont-store-that-in-a-float/
void InteractiveRunner::mainLoop()
{
	log->logInfo("Entering the main loop");

	double timeStep = 1.0 / settings->runner.updateFrequency;
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

	if (keyWasPressed(GLFW_KEY_F1))
		settings->runner.showFps = !settings->runner.showFps;

	if (keyWasPressed(GLFW_KEY_F2))
		settings->runner.showCameraInfo = !settings->runner.showCameraInfo;

	if (keyWasPressed(GLFW_KEY_F9))
	{
		settings->framebuffer.enableSmoothing = !settings->framebuffer.enableSmoothing;
		framebuffer->enableSmoothing(settings->framebuffer.enableSmoothing);
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
			runnerStates[currentState]->framebufferResized(framebuffer->getWidth(), framebuffer->getHeight());
		}
	}

	if (keyWasPressed(GLFW_KEY_F11) && settings->framebuffer.enableResizing)
	{
		if (settings->framebuffer.resizeScale < 1.0)
		{
			settings->framebuffer.resizeScale *= 2.0;

			if (settings->framebuffer.resizeScale > 1.0)
				settings->framebuffer.resizeScale = 1.0;

			framebuffer->setSize((int)(windowWidth * settings->framebuffer.resizeScale + 0.5), (int)(windowHeight * settings->framebuffer.resizeScale + 0.5));
			runnerStates[currentState]->framebufferResized(framebuffer->getWidth(), framebuffer->getHeight());
		}
	}

	if (currentState != RunnerStates::None)
		runnerStates[currentState]->update(timeStep);
	else
		throw std::runtime_error("App state has not been set");
}

void InteractiveRunner::render(double timeStep, double interpolation)
{
	renderFpsCounter.countFrame();

	if (currentState != RunnerStates::None)
		runnerStates[currentState]->render(timeStep, interpolation);
	else
		throw std::runtime_error("App state has not been set");

	if (settings->runner.showFps)
		infoFont->drawText(*framebuffer, 5, framebuffer->getHeight() - settings->runner.infoFontSize - 2, renderFpsCounter.getFpsString(), Color(255, 255, 255, 200));

	if (keyWasPressed(GLFW_KEY_F8))
	{
		log->logInfo("Saving a screenshot to screenshot.png");

		Image image = Image(*framebuffer);
		image.saveAs("screenshot.png");
	}

	framebuffer->render();
	framebuffer->clear();

	glfwSwapBuffers(glfwWindow);
}
