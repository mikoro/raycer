// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Rendering/Framebuffer.h"
#include "Runners/WindowRunner.h"
#include "Runners/ConsoleRunner.h"
#include "Runners/NetworkRunner.h"
#include "OpenCL/CLManager.h"
#include "OpenCL/CLTracer.h"

#ifdef RUN_UNIT_TESTS
#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"
#endif

using namespace Raycer;

int main(int argc, char** argv)
{
#ifdef RUN_UNIT_TESTS
	return Catch::Session().run(argc, argv);
#else
	return App().run(argc, argv);
#endif
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

#ifdef __APPLE__
// change directory to Resources when run as an app bundle
void changeDirectory()
{
	CFBundleRef bundle = CFBundleGetMainBundle();

	if (!bundle)
		return;

	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(bundle);
	CFStringRef last = CFURLCopyLastPathComponent(resourcesURL);

	if (CFStringCompare(CFSTR("Resources"), last, 0) != kCFCompareEqualTo)
	{
		CFRelease(last);
		CFRelease(resourcesURL);
		return;
	}

	CFRelease(last);
	char resourcesPath[1024];

	if (!CFURLGetFileSystemRepresentation(resourcesURL, true, (UInt8*)resourcesPath, 1024))
	{
		CFRelease(resourcesURL);
		return;
	}

	CFRelease(resourcesURL);
	chdir(resourcesPath);
}
#endif

int App::run(int argc, char** argv)
{
#ifdef _WIN32
	SetConsoleCtrlHandler(PHANDLER_ROUTINE(consoleCtrlHandler), TRUE);
#endif

#ifdef __APPLE__
	changeDirectory();
#endif

	TCLAP::CmdLine cmd("Raycer", ' ', "1.0.0");
	TCLAP::SwitchArg interactiveSwitch("", "interactive", "View the scene interactively", cmd, false);
	TCLAP::SwitchArg nonInteractiveSwitch("", "non-interactive", "Force non-interactive mode", cmd, false);
	TCLAP::SwitchArg enableOpenCLSwitch("", "opencl", "Use OpenCL for raytracing", cmd, false);
	TCLAP::ValueArg<size_t> clPlatformIdArg("", "cl-platform", "OpenCL platform to use", false, 0, "int", cmd);
	TCLAP::ValueArg<uint> clDeviceTypeArg("", "cl-device-type", "OpenCL device type to use", false, 0, "int", cmd);
	TCLAP::ValueArg<size_t> clDeviceIdArg("", "cl-device", "OpenCL device to use", false, 0, "int", cmd);
	TCLAP::ValueArg<uint> testSceneArg("", "test", "Select one of the test scenes", false, 0, "int", cmd);
	TCLAP::SwitchArg nonTestSceneSwitch("", "non-test", "Force test scene mode off", cmd, false);
	TCLAP::SwitchArg clientSwitch("", "client", "Enable network client mode", cmd, false);
	TCLAP::SwitchArg serverSwitch("", "server", "Enable network server mode", cmd, false);
	TCLAP::ValueArg<std::string> sceneFileNameArg("s", "scene", "Path to the scene file", false, "", "string", cmd);
	TCLAP::ValueArg<size_t> widthArg("w", "width", "Width of the output image or window", false, 0, "int", cmd);
	TCLAP::ValueArg<size_t> heightArg("h", "height", "Height of the output image or window", false, 0, "int", cmd);
	TCLAP::ValueArg<std::string> outputFileNameArg("o", "output", "Path to the output image file", false, "", "string", cmd);
	TCLAP::SwitchArg autoViewSwitch("", "view", "Open the image automatically after completion", cmd, false);

	try
	{
		cmd.parse(argc, argv);
	}
	catch (const TCLAP::ArgException& ex)
	{
		std::cerr << "Command line parsing error: " << ex.error() << " for arg " << ex.argId() << std::endl;
		return -1;
	}

	try
	{
		Settings& settings = getSettings();
		WindowRunner& windowRunner = getWindowRunner();
		ConsoleRunner& consoleRunner = getConsoleRunner();
		NetworkRunner& networkRunner = getNetworkRunner();

		settings.load("settings.ini");

		if (interactiveSwitch.isSet())
			settings.general.interactive = true;

		if (nonInteractiveSwitch.isSet())
			settings.general.interactive = false;

		if (enableOpenCLSwitch.isSet())
			settings.openCL.enabled = true;

		if (clPlatformIdArg.isSet())
			settings.openCL.platformId = clPlatformIdArg.getValue();

		if (clDeviceTypeArg.isSet())
			settings.openCL.deviceType = clDeviceTypeArg.getValue();

		if (clDeviceIdArg.isSet())
			settings.openCL.deviceId = clDeviceIdArg.getValue();

		if (testSceneArg.isSet())
		{
			settings.scene.enableTestScenes = true;
			settings.scene.testSceneNumber = testSceneArg.getValue();
		}

		if (nonTestSceneSwitch.isSet())
			settings.scene.enableTestScenes = false;

		if (clientSwitch.isSet())
		{
			settings.network.isClient = true;
			settings.network.isServer = false;
			settings.general.interactive = false;
		}

		if (serverSwitch.isSet())
		{
			settings.network.isServer = true;
			settings.network.isClient = false;
			settings.general.interactive = false;
		}

		if (sceneFileNameArg.isSet())
			settings.scene.fileName = sceneFileNameArg.getValue();

		if (widthArg.isSet())
			settings.image.width = settings.window.width = widthArg.getValue();

		if (heightArg.isSet())
			settings.image.height = settings.window.height = heightArg.getValue();

		if (outputFileNameArg.isSet())
			settings.image.fileName = outputFileNameArg.getValue();

		if (autoViewSwitch.isSet())
			settings.image.autoView = true;

		omp_set_num_threads(settings.general.maxThreadCount);

		if (settings.general.interactive)
			return windowRunner.run();
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

WindowRunner& App::getWindowRunner()
{
	static WindowRunner windowRunner;
	return windowRunner;
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

CLManager& App::getCLManager()
{
	static CLManager clManager;
	return clManager;
}

CLTracer& App::getCLTracer()
{
	static CLTracer clTracer;
	return clTracer;
}
