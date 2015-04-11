// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <chrono>
#include <ctime>
#include <iomanip>

#include "tclap/CmdLine.h"

#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Rendering/Framebuffer.h"
#include "GpuRaytracing/OpenCL.h"
#include "Runners/InteractiveRunner.h"
#include "Runners/ConsoleRunner.h"
#include "CpuRaytracing/CpuRaytracer.h"
#include "GpuRaytracing/GpuRaytracer.h"

using namespace Raycer;

int main(int argc, char** argv)
{
	return App().run(argc, argv);
}

int App::run(int argc, char** argv)
{
	TCLAP::CmdLine cmd("Raycer", ' ', "1.0.0");
	TCLAP::ValueArg<std::string> sceneFileNameArg("s", "scene", "Path to the scene file", false, "", "string", cmd);
	TCLAP::ValueArg<std::string> outputFileNameArg("o", "output", "Path to the output image file", false, "", "string", cmd);
	TCLAP::ValueArg<size_t> widthArg("w", "width", "Width of the output image or window", false, 1280, "int", cmd);
	TCLAP::ValueArg<size_t> heightArg("h", "height", "Height of the output image or window", false, 800, "int", cmd);
	TCLAP::ValueArg<int> platformIdArg("p", "platform", "OpenCL platform to use", false, 0, "int", cmd);
	TCLAP::ValueArg<int> deviceIdArg("d", "device", "OpenCL device to use", false, 0, "int", cmd);
	TCLAP::SwitchArg interactiveSwitch("i", "interactive", "View the scene interactively", cmd, false);
	TCLAP::SwitchArg autoViewSwitch("v", "view", "View the image after completion", cmd, false);
	TCLAP::SwitchArg useOpenClSwitch("g", "gpu", "Use OpenCL for raytracing (GPU)", cmd, false);

	try
	{
		cmd.parse(argc, argv);
	}
	catch (const TCLAP::ArgException &ex)
	{
		std::cerr << "Command line parsing error: " << ex.error() << " for arg " << ex.argId() << std::endl;
		return -1;
	}

	try
	{
		Log& log = getLog();
		log.logInfo("Starting application");

		Settings& settings = getSettings();
		InteractiveRunner& interactiveRunner = getInteractiveRunner();
		ConsoleRunner& consoleRunner = getConsoleRunner();

		settings.load("settings.ini");

		if (sceneFileNameArg.isSet())
			settings.scene.fileName = sceneFileNameArg.getValue();

		if (outputFileNameArg.isSet())
			settings.image.fileName = outputFileNameArg.getValue();

		if (widthArg.isSet())
		{
			settings.image.width = widthArg.getValue();
			settings.window.width = widthArg.getValue();
		}
		
		if (heightArg.isSet())
		{
			settings.image.height = heightArg.getValue();
			settings.window.height = heightArg.getValue();
		}

		if (platformIdArg.isSet())
			settings.openCl.platformId = platformIdArg.getValue();

		if (deviceIdArg.isSet())
			settings.openCl.deviceId = deviceIdArg.getValue();

		if (interactiveSwitch.isSet())
			settings.general.interactive = interactiveSwitch.getValue();

		if (autoViewSwitch.isSet())
			settings.image.autoView = autoViewSwitch.getValue();

		if (useOpenClSwitch.isSet())
			settings.openCl.enabled = useOpenClSwitch.getValue();

		if (settings.general.interactive)
			return interactiveRunner.run();
		else
			return consoleRunner.run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return -1;
	}
}

Log& App::getLog()
{
	static Log log("raycer.log");
	return log;
}

Settings& App::getSettings()
{
	static Settings settings;
	return settings;
}

Framebuffer& App::getFramebuffer()
{
	static Framebuffer framebuffer;
	return framebuffer;
}

OpenCL& App::getOpenCL()
{
	static OpenCL openCL;
	return openCL;
}

InteractiveRunner& App::getInteractiveRunner()
{
	static InteractiveRunner interactiveRunner;
	return interactiveRunner;
}

ConsoleRunner& App::getConsoleRunner()
{
	static ConsoleRunner consoleRunner;
	return consoleRunner;
}

CpuRaytracer& App::getCpuRaytracer()
{
	static CpuRaytracer cpuRaytracer;
	return cpuRaytracer;
}

GpuRaytracer& App::getGpuRaytracer()
{
	static GpuRaytracer gpuRaytracer;
	return gpuRaytracer;
}
