// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Log;
	class Settings;
	class Framebuffer;
	class OpenCL;
	class InteractiveRunner;
	class ConsoleRunner;
	class CpuRaytracer;
	class GpuRaytracer;

	class App
	{
	public:

		int run(int argc, char** argv);

		static Log& getLog();
		static Settings& getSettings();
		static Framebuffer& getFramebuffer();
		static OpenCL& getOpenCL();
		static InteractiveRunner& getInteractiveRunner();
		static ConsoleRunner& getConsoleRunner();
		static CpuRaytracer& getCpuRaytracer();
		static GpuRaytracer& getGpuRaytracer();
	};
}
