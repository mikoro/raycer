// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Utils/Settings.h"
#include "Utils/IniReader.h"

using namespace Raycer;

void Settings::load(const std::string& fileName)
{
	IniReader iniReader;
	iniReader.readFile(fileName);

	general.interactive = iniReader.getValue<bool>("general", "interactive");

	openCl.enabled = iniReader.getValue<bool>("openCl", "enabled");
	openCl.platformId = iniReader.getValue<int>("openCl", "platformId");
	openCl.deviceId = iniReader.getValue<int>("openCl", "deviceId");
	openCl.deviceType = iniReader.getValue<int>("openCl", "deviceType");

	scene.fileName = iniReader.getValue<std::string>("scene", "fileName");

	image.width = iniReader.getValue<size_t>("image", "width");
	image.height = iniReader.getValue<size_t>("image", "height");
	image.fileName = iniReader.getValue<std::string>("image", "fileName");
	image.autoView = iniReader.getValue<bool>("image", "autoView");

	window.width = iniReader.getValue<size_t>("window", "width");
	window.height = iniReader.getValue<size_t>("window", "height");
	window.fullscreen = iniReader.getValue<bool>("window", "fullscreen");
	window.vsync = iniReader.getValue<bool>("window", "vsync");
	window.hideCursor = iniReader.getValue<bool>("window", "hideCursor");
	window.showFps = iniReader.getValue<bool>("window", "showFps");
	window.showCameraInfo = iniReader.getValue<bool>("window", "showCameraInfo");
	window.infoFont = iniReader.getValue<std::string>("window", "infoFont");
	window.infoFontSize = iniReader.getValue<int>("window", "infoFontSize");

	framebuffer.scale = iniReader.getValue<double>("framebuffer", "scale");
	framebuffer.smoothing = iniReader.getValue<bool>("framebuffer", "smoothing");

	controls.moveSpeed = iniReader.getValue<double>("controls", "moveSpeed");
	controls.mouseSpeed = iniReader.getValue<double>("controls", "mouseSpeed");
	controls.freeLook = iniReader.getValue<bool>("controls", "freeLook");
}
