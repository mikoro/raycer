// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <map>
#include <memory>

#include "Runners/InteractiveSettings.h"
#include "States/InteractiveState.h"
#include "Utils/Log.h"
#include "Utils/IniReader.h"
#include "Rendering/Framebuffer.h"
#include "Rendering/Font.h"
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

	enum class RunnerStates { None, TraceFast };

	class InteractiveRunner
	{
	public:

		InteractiveRunner(BaseLog& baseLog);

		int run();
		void stop();
		
		GLFWwindow* getGlfwWindow() const;
		int getWindowWidth() const;
		int getWindowHeight() const;
		const MouseInfo& getMouseInfo() const;
		Font& getInfoFont() const;

		bool keyIsDown(int key);
		bool mouseIsDown(int button);
		bool keyWasPressed(int key);

		void changeState(RunnerStates newState);

	private:

		void initialize();
		void shutdown();

		void windowResized(int width, int height);

		void mainLoop();
		void update(double timeStep);
		void render(double timeStep, double interpolation);

		std::unique_ptr<NamedLog> log;
		std::unique_ptr<IniReader> iniReader;
		std::unique_ptr<InteractiveSettings> settings;
		std::unique_ptr<Framebuffer> framebuffer;
		std::unique_ptr<Font> infoFont;

		BaseLog& baseLog;
		bool shouldRun = true;
		bool glfwInitialized = false;
		GLFWwindow* glfwWindow = nullptr;
		int windowWidth = 0;
		int windowHeight = 0;
		int previousMouseX = 0;
		int previousMouseY = 0;
		MouseInfo mouseInfo;
		std::map<int, bool> keyStates;
		std::map<RunnerStates, std::unique_ptr<InteractiveState>> runnerStates;
		RunnerStates currentState = RunnerStates::None;
		FpsCounter updateFpsCounter;
		FpsCounter renderFpsCounter;
	};
}
