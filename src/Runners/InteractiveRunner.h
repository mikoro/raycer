// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <map>
#include <memory>

#include "InteractiveStates/InteractiveState.h"
#include "Rendering/Text.h"
#include "Utils/FpsCounter.h"

struct GLFWwindow;

namespace Raycer
{
	struct MouseInfo
	{
		int windowX = 0;
		int windowY = 0;
		int framebufferX = 0;
		int framebufferY = 0;
		int deltaX = 0;
		int deltaY = 0;
	};

	enum class RunnerStates { None, Raytracing };

	class InteractiveRunner
	{
	public:

		InteractiveRunner();

		int run();
		void stop();
		void pause();

		GLFWwindow* getGlfwWindow() const;
		int getWindowWidth() const;
		int getWindowHeight() const;
		const MouseInfo& getMouseInfo() const;
		Text& getDefaultText();
		double getElapsedTime() const;

		bool keyIsDown(int key);
		bool mouseIsDown(int button);
		bool keyWasPressed(int key);

		void changeState(RunnerStates newState);

	private:

		InteractiveRunner(const InteractiveRunner& interactiveRunner);
		InteractiveRunner& operator=(const InteractiveRunner& interactiveRunner);

		void initialize();
		void shutdown();

		void windowResized(int width, int height);
		void resizeFramebufferImage(int width, int height);

		void mainLoop();
		void update(double timeStep);
		void render(double timeStep, double interpolation);

		void takeScreenshot() const;

		bool shouldRun = true;
		bool isPaused = false;
		bool glfwInitialized = false;
		GLFWwindow* glfwWindow = nullptr;
		int windowWidth = 0;
		int windowHeight = 0;
		int previousMouseX = 0;
		int previousMouseY = 0;
		double startTime = 0.0;
		MouseInfo mouseInfo;
		std::map<int, bool> keyStates;
		std::map<RunnerStates, std::unique_ptr<InteractiveState>> runnerStates;
		RunnerStates currentState = RunnerStates::None;
		Text defaultText;
		Text pauseText;
		FpsCounter updateFpsCounter;
		FpsCounter renderFpsCounter;
	};
}
