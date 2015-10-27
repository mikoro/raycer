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
		int64_t scaledX = 0;
		int64_t scaledY = 0;
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
		uint64_t getWindowWidth() const;
		uint64_t getWindowHeight() const;
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

		void initialize();
		void shutdown();

		void windowResized(uint64_t width, uint64_t height);

		void mainLoop();
		void update(double timeStep);
		void render(double timeStep, double interpolation);

		void takeScreenshot() const;

		bool shouldRun = true;
		bool isPaused = false;
		bool glfwInitialized = false;

		double startTime = 0.0;

		GLFWwindow* glfwWindow = nullptr;
		uint64_t windowWidth = 0;
		uint64_t windowHeight = 0;

		MouseInfo mouseInfo;
		int64_t previousMouseX = 0;
		int64_t previousMouseY = 0;

		std::map<int32_t, bool> keyStates;
		std::map<int32_t, bool> mouseStates;

		std::map<WindowRunnerStates, std::unique_ptr<WindowRunnerState>> runnerStates;
		WindowRunnerStates currentState = WindowRunnerStates::NONE;

		Text defaultText;
		Text pauseText;

		FpsCounter fpsCounter;
	};
}
