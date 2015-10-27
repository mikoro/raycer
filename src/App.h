// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Log;
	class Settings;
	class WindowRunner;
	class ConsoleRunner;
	class NetworkRunner;
	class CLManager;
	class CLTracer;

	class App
	{
	public:

		int run(int argc, char** argv);

		static Log& getLog();
		static Settings& getSettings();
		static WindowRunner& getWindowRunner();
		static ConsoleRunner& getConsoleRunner();
		static NetworkRunner& getNetworkRunner();
		static CLManager& getCLManager();
		static CLTracer& getCLTracer();
	};
}
