// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <map>
#include <memory>

#include "Settings.h"
#include "App/AppStateBase.h"
#include "Utils/Log.h"
#include "Utils/IniReader.h"
#include "Framebuffer/Framebuffer.h"
#include "Framebuffer/Font.h"
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

	enum class AppStates { None, Test };

	class App
	{
	public:

		int run();
		void stop();
		
		GLFWwindow* getGlfwWindow() const;
		int getWindowWidth() const;
		int getWindowHeight() const;
		BaseLog& getBaseLog() const;
		const MouseInfo& getMouseInfo() const;
		bool keyIsDown(int key);
		bool mouseIsDown(int button);
		bool keyWasPressed(int key);
		void changeState(AppStates newState);

	private:

		void initialize();
		void shutdown();

		void windowResized(int width, int height);

		void mainLoop();
		void update(double timeStep);
		void render(double timeStep, double interpolation);

		std::unique_ptr<BaseLog> baseLog;
		std::unique_ptr<NamedLog> log;
		std::unique_ptr<IniReader> iniReader;
		std::unique_ptr<Settings> settings;
		std::unique_ptr<Framebuffer> framebuffer;
		std::unique_ptr<Font> fpsFont;

		bool shouldRun = true;
		bool glfwInitialized = false;
		GLFWwindow* glfwWindow = nullptr;
		int windowWidth = 0;
		int windowHeight = 0;
		int previousMouseX = 0;
		int previousMouseY = 0;
		MouseInfo mouseInfo;
		std::map<int, bool> keyStates;
		std::map<AppStates, std::unique_ptr<AppStateBase>> appStates;
		AppStates currentState = AppStates::None;
		FpsCounter updateFpsCounter;
		FpsCounter renderFpsCounter;
	};
}
