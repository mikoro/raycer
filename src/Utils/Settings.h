// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

namespace Raycer
{
	class Settings
	{
	public:

		void load(const std::string& fileName);

		struct General
		{
			bool interactive;
			int maxThreadCount;
			bool checkGLErrors;
			bool checkCLErrors;
		} general;

		struct OpenCL
		{
			bool enabled;
			int platformId;
			int deviceType;
			int deviceId;
			std::string options;
		} openCL;

		struct Network
		{
			bool isClient;
			bool isServer;
			std::string localAddress;
			int localPort;
			std::string broadcastAddress;
			int broadcastPort;
		} network;

		struct Scene
		{
			std::string fileName;
			bool enableTestScene;
			int testSceneNumber;
		} scene;

		struct Image
		{
			int width;
			int height;
			std::string fileName;
			bool autoView;
		} image;

		struct Window
		{
			int width;
			int height;
			bool fullscreen;
			bool vsync;
			bool hideCursor;
			bool showFps;
			bool showCameraInfo;
			std::string defaultFont;
			int defaultFontSize;
		} window;

		struct Framebuffer
		{
			double scale;
			bool smoothing;
			std::string resampleVertexShader;
			std::string resampleFragmentShader;
			std::string filterVertexShader;
			std::string filterFragmentShader;
		} framebuffer;

		struct Camera
		{
			double moveSpeed;
			double moveDrag;
			double mouseSpeed;
			double mouseDrag;
			double slowModifier;
			double fastModifier;
			double veryFastModifier;
			bool freeLook;
		} camera;
	};
}
