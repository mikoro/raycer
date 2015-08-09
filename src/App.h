// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Log;
	class Settings;
	class Framebuffer;
	class CLManager;
	class WindowRunner;
	class ConsoleRunner;
	class NetworkRunner;
	class Raytracer;
	class CLRaytracer;

	class App
	{
	public:

		int run(int argc, char** argv);

		static Log& getLog();
		static Settings& getSettings();
		static Framebuffer& getFramebuffer();
		static CLManager& getCLManager();
		static WindowRunner& getWindowRunner();
		static ConsoleRunner& getConsoleRunner();
		static NetworkRunner& getNetworkRunner();
		static Raytracer& getRaytracer();
		static CLRaytracer& getCLRaytracer();
	};
}
