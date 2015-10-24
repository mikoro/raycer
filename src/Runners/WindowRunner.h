// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <map>
#include <memory>

#include "Rendering/Text.h"
#include "Utils/FpsCounter.h"
#include "Runners/WindowRunnerStates/WindowRunnerState.h"

struct GLFWwindow;

namespace Raycer
{
	struct MouseInfo
	{
		int64_t windowX = 0;
		int64_t windowY = 0;
		int64_t framebufferX = 0;
		int64_t framebufferY = 0;
		int64_t deltaX = 0;
		int64_t deltaY = 0;
		double scrollY = 0.0;
		bool hasScrolled = false;
	};

	class WindowRunner
	{
	public:

		WindowRunner();
		~WindowRunner();

		int run();
		void stop();
		void pause();

		GLFWwindow* getGlfwWindow() const;
		size_t getWindowWidth() const;
		size_t getWindowHeight() const;
		const MouseInfo& getMouseInfo() const;
		Text& getDefaultText();
		double getElapsedTime() const;
		double getFps() const;

		bool keyIsDown(int32_t key);
		bool mouseIsDown(int32_t button);
		bool keyWasPressed(int32_t key);
		bool mouseWasPressed(int32_t button);
		double getMouseWheelScroll();

		void changeState(WindowRunnerStates newState);

	private:

		WindowRunner(const WindowRunner& windowRunner);
		WindowRunner& operator=(const WindowRunner& windowRunner);

		void initialize();
		void shutdown();

		void windowResized(size_t width, size_t height);
		void resizeFramebuffer(size_t width, size_t height);

		void mainLoop();
		void update(double timeStep);
		void render(double timeStep, double interpolation);

		void takeScreenshot() const;

		bool shouldRun = true;
		bool isPaused = false;
		bool glfwInitialized = false;
		GLFWwindow* glfwWindow = nullptr;
		size_t windowWidth = 0;
		size_t windowHeight = 0;
		int64_t previousMouseX = 0;
		int64_t previousMouseY = 0;
		double startTime = 0.0;
		MouseInfo mouseInfo;
		std::map<int32_t, bool> keyStates;
		std::map<int32_t, bool> mouseStates;
		std::map<WindowRunnerStates, std::shared_ptr<WindowRunnerState>> runnerStates;
		WindowRunnerStates currentState = WindowRunnerStates::None;
		Text defaultText;
		Text pauseText;
		FpsCounter updateFpsCounter;
		FpsCounter renderFpsCounter;
	};
}
