// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Settings.h"
#include "Utils/IniReader.h"

using namespace Raycer;

void Settings::load(const std::string& fileName)
{
	IniReader iniReader;
	iniReader.readFile(fileName);

	general.interactive = iniReader.getValue<bool>("general", "interactive");
	general.maxThreadCount = iniReader.getValue<uint>("general", "maxThreadCount");
	general.checkGLErrors = iniReader.getValue<bool>("general", "checkGLErrors");
	general.checkCLErrors = iniReader.getValue<bool>("general", "checkCLErrors");

	openCL.enabled = iniReader.getValue<bool>("openCL", "enabled");
	openCL.platformId = iniReader.getValue<size_t>("openCL", "platformId");
	openCL.deviceType = iniReader.getValue<uint>("openCL", "deviceType");
	openCL.deviceId = iniReader.getValue<size_t>("openCL", "deviceId");
	openCL.options = iniReader.getValue("openCL", "options");

	network.isClient = iniReader.getValue<bool>("network", "isClient");
	network.isServer = iniReader.getValue<bool>("network", "isServer");
	network.localAddress = iniReader.getValue("network", "localAddress");
	network.localPort = iniReader.getValue<uint>("network", "localPort");
	network.broadcastAddress = iniReader.getValue("network", "broadcastAddress");
	network.broadcastPort = iniReader.getValue<uint>("network", "broadcastPort");

	scene.fileName = iniReader.getValue("scene", "fileName");
	scene.enableTestScenes = iniReader.getValue<bool>("scene", "enableTestScenes");
	scene.testSceneNumber = iniReader.getValue<int>("scene", "testSceneNumber");

	image.width = iniReader.getValue<size_t>("image", "width");
	image.height = iniReader.getValue<size_t>("image", "height");
	image.fileName = iniReader.getValue("image", "fileName");
	image.autoView = iniReader.getValue<bool>("image", "autoView");

	window.width = iniReader.getValue<size_t>("window", "width");
	window.height = iniReader.getValue<size_t>("window", "height");
	window.enableFullscreen = iniReader.getValue<bool>("window", "enableFullscreen");
	window.enableVsync = iniReader.getValue<bool>("window", "enableVsync");
	window.hideCursor = iniReader.getValue<bool>("window", "hideCursor");
	window.showInfoText = iniReader.getValue<bool>("window", "showInfoText");
	window.defaultFont = iniReader.getValue("window", "defaultFont");
	window.defaultFontSize = iniReader.getValue<size_t>("window", "defaultFontSize");

	framebuffer.scale = iniReader.getValue<double>("framebuffer", "scale");
	framebuffer.enableSmoothing = iniReader.getValue<bool>("framebuffer", "enableSmoothing");
	framebuffer.resampleVertexShader = iniReader.getValue("framebuffer", "resampleVertexShader");
	framebuffer.resampleFragmentShader = iniReader.getValue("framebuffer", "resampleFragmentShader");
	framebuffer.filterVertexShader = iniReader.getValue("framebuffer", "filterVertexShader");
	framebuffer.filterFragmentShader = iniReader.getValue("framebuffer", "filterFragmentShader");

	camera.freeLook = iniReader.getValue<bool>("camera", "freeLook");
	camera.smoothMovement = iniReader.getValue<bool>("camera", "smoothMovement");
	camera.moveSpeed = iniReader.getValue<double>("camera", "moveSpeed");
	camera.moveDrag = iniReader.getValue<double>("camera", "moveDrag");
	camera.mouseSpeed = iniReader.getValue<double>("camera", "mouseSpeed");
	camera.mouseDrag = iniReader.getValue<double>("camera", "mouseDrag");
	camera.slowModifier = iniReader.getValue<double>("camera", "slowModifier");
	camera.fastModifier = iniReader.getValue<double>("camera", "fastModifier");
	camera.veryFastModifier = iniReader.getValue<double>("camera", "veryFastModifier");
}
