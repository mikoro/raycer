// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "tclap/CmdLine.h"

#include "App/App.h"
#include "Utils/ConsoleRunner.h"

using namespace Raycer;

int main(int argc, char** argv)
{
	TCLAP::CmdLine cmd("Raycer", ' ', "1.0.0");
	TCLAP::ValueArg<std::string> sceneFileArg("s", "scene", "Path to the scene file", false, "", "string", cmd);
	TCLAP::ValueArg<std::string> outputFileArg("o", "output", "Path to the output image file", false, "output.png", "string", cmd);
	TCLAP::ValueArg<int> widthArg("w", "width", "Width of the output image", false, 1280, "int", cmd);
	TCLAP::ValueArg<int> heightArg("h", "height", "Height of the output image", false, 800, "int", cmd);
	TCLAP::SwitchArg interactiveSwitch("i", "interactive", "View the scene interactively", cmd, false);
	TCLAP::SwitchArg viewSwitch("v", "view", "View the image after completion", cmd, false);

	try
	{
		cmd.parse(argc, argv);
	}
	catch (const TCLAP::ArgException &ex)
	{
		std::cerr << "Command line parsing error: " << ex.error() << " for arg " << ex.argId() << std::endl;
		return -1;
	}

	if (interactiveSwitch.getValue())
		return App().run();

	try
	{
		ConsoleRunnerSettings settings;

		settings.sceneFileName = sceneFileArg.getValue();
		settings.outputFileName = outputFileArg.getValue();
		settings.width = widthArg.getValue();
		settings.height = heightArg.getValue();
		settings.viewImage = viewSwitch.getValue();

		return ConsoleRunner::run(settings);
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Unhandled exception: " << ex.what() << std::endl;
		return -1;
	}
}
