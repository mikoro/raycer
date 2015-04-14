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
#include "Runners/NetworkRunner.h"
#include "CpuRaytracing/CpuRaytracer.h"
#include "GpuRaytracing/GpuRaytracer.h"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace Raycer;

int main(int argc, char** argv)
{
	return App().run(argc, argv);
}

#ifdef _WIN32
BOOL consoleCtrlHandler(DWORD fdwCtrlType)
{
	if (fdwCtrlType == CTRL_C_EVENT)
	{
		App::getConsoleRunner().interrupt();
		App::getNetworkRunner().interrupt();

		return true;
	}
	else
		return false;
}
#endif

int App::run(int argc, char** argv)
{
#ifdef _WIN32
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)consoleCtrlHandler, TRUE);
#endif

	TCLAP::CmdLine cmd("Raycer", ' ', "1.0.0");
	TCLAP::SwitchArg interactiveSwitch("i", "interactive", "View the scene interactively", cmd, false);
	TCLAP::SwitchArg useOpenCLSwitch("l", "opencl", "Use OpenCL for raytracing", cmd, false);
	TCLAP::ValueArg<int> platformIdArg("p", "platform", "OpenCL platform to use", false, 0, "int", cmd);
	TCLAP::ValueArg<int> deviceTypeArg("t", "type", "OpenCL device type to use", false, 0, "int", cmd);
	TCLAP::ValueArg<int> deviceIdArg("d", "device", "OpenCL device to use", false, 0, "int", cmd);
	TCLAP::SwitchArg clientSwitch("c", "client", "Enable network client mode", cmd, false);
	TCLAP::SwitchArg serverSwitch("e", "server", "Enable network server mode", cmd, false);
	TCLAP::ValueArg<std::string> sceneFileNameArg("s", "scene", "Path to the scene file", false, "", "string", cmd);
	TCLAP::ValueArg<int> widthArg("w", "width", "Width of the output image or window", false, 0, "int", cmd);
	TCLAP::ValueArg<int> heightArg("h", "height", "Height of the output image or window", false, 0, "int", cmd);
	TCLAP::ValueArg<std::string> outputFileNameArg("o", "output", "Path to the output image file", false, "", "string", cmd);
	TCLAP::SwitchArg autoViewSwitch("v", "view", "View the image after completion", cmd, false);
	
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
		Settings& settings = getSettings();
		InteractiveRunner& interactiveRunner = getInteractiveRunner();
		ConsoleRunner& consoleRunner = getConsoleRunner();
		NetworkRunner& networkRunner = getNetworkRunner();

		settings.load("settings.ini");

		if (interactiveSwitch.isSet())
			settings.general.interactive = interactiveSwitch.getValue();

		if (useOpenCLSwitch.isSet())
			settings.openCL.enabled = useOpenCLSwitch.getValue();

		if (platformIdArg.isSet())
			settings.openCL.platformId = platformIdArg.getValue();

		if (deviceTypeArg.isSet())
			settings.openCL.deviceType = deviceTypeArg.getValue();

		if (deviceIdArg.isSet())
			settings.openCL.deviceId = deviceIdArg.getValue();

		if (clientSwitch.isSet())
		{
			settings.network.isClient = clientSwitch.getValue();
			settings.network.isServer = !settings.network.isClient;
		}

		if (serverSwitch.isSet())
		{
			settings.network.isServer = serverSwitch.getValue();
			settings.network.isClient = !settings.network.isServer;
		}

		if (sceneFileNameArg.isSet())
			settings.scene.fileName = sceneFileNameArg.getValue();

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

		if (outputFileNameArg.isSet())
			settings.image.fileName = outputFileNameArg.getValue();

		if (autoViewSwitch.isSet())
			settings.image.autoView = autoViewSwitch.getValue();

		if (settings.general.interactive)
			return interactiveRunner.run();
		else
		{
			if (settings.network.isClient || settings.network.isServer)
				return networkRunner.run();
			else
				return consoleRunner.run();
		}
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

NetworkRunner& App::getNetworkRunner()
{
	static NetworkRunner networkRunner;
	return networkRunner;
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
