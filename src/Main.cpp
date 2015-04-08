// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "tclap/CmdLine.h"

#include "Runners/InteractiveRunner.h"
#include "Runners/ConsoleRunner.h"
#include "Utils/Log.h"

using namespace Raycer;

int main(int argc, char** argv)
{
	TCLAP::CmdLine cmd("Raycer", ' ', "1.0.0");
	TCLAP::ValueArg<std::string> sceneFileNameArg("s", "scene", "Path to the scene file", true, "", "string", cmd);
	TCLAP::ValueArg<std::string> outputFileNameArg("o", "output", "Path to the output image file", false, "output.png", "string", cmd);
	TCLAP::ValueArg<int> widthArg("w", "width", "Width of the output image", false, 1280, "int", cmd);
	TCLAP::ValueArg<int> heightArg("h", "height", "Height of the output image", false, 800, "int", cmd);
	TCLAP::ValueArg<int> platformIdArg("p", "platform", "OpenCL platform to use", false, 0, "int", cmd);
	TCLAP::ValueArg<int> deviceIdArg("d", "device", "OpenCL device to use", false, 0, "int", cmd);
	TCLAP::SwitchArg interactiveSwitch("i", "interactive", "View the scene interactively (view settings.ini for more settings)", cmd, false);
	TCLAP::SwitchArg viewImageSwitch("v", "view", "View the image after completion", cmd, false);
	TCLAP::SwitchArg useOpenClSwitch("g", "gpu", "Use GPU (or other OpenCL device) for raytracing", cmd, false);

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
		BaseLog baseLog = BaseLog("raycer.log");
		ConsoleSettings settings;

		settings.sceneFileName = sceneFileNameArg.getValue();
		settings.outputFileName = outputFileNameArg.getValue();
		settings.width = widthArg.getValue();
		settings.height = heightArg.getValue();
		settings.interactive = interactiveSwitch.getValue();
		settings.viewImage = viewImageSwitch.getValue();
		settings.useOpenCl = useOpenClSwitch.getValue();

		if (settings.interactive)
			return InteractiveRunner(baseLog).run(settings);
		else
			return ConsoleRunner(baseLog).run(settings);
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Unhandled exception: " << ex.what() << std::endl;
		return -1;
	}
}
