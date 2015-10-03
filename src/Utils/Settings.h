// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Settings
	{
	public:

		void load(const std::string& fileName);

		struct General
		{
			bool interactive;
			uint maxThreadCount;
			bool checkGLErrors;
			bool checkCLErrors;
		} general;

		struct OpenCL
		{
			bool enabled;
			size_t platformId;
			uint deviceType;
			size_t deviceId;
			std::string options;
		} openCL;

		struct Network
		{
			bool isClient;
			bool isServer;
			std::string localAddress;
			uint localPort;
			std::string broadcastAddress;
			uint broadcastPort;
		} network;

		struct Scene
		{
			std::string fileName;
			bool enableTestScenes;
			uint testSceneNumber;
		} scene;

		struct Image
		{
			size_t width;
			size_t height;
			std::string fileName;
			bool autoView;
		} image;

		struct Window
		{
			size_t width;
			size_t height;
			bool enableFullscreen;
			bool enableVsync;
			bool hideCursor;
			bool showInfoText;
			std::string defaultFont;
			size_t defaultFontSize;
		} window;

		struct Framebuffer
		{
			double scale;
			bool enableSmoothing;
			std::string resampleVertexShader;
			std::string resampleFragmentShader;
			std::string filterVertexShader;
			std::string filterFragmentShader;
		} framebuffer;

		struct Camera
		{
			bool freeLook;
			bool smoothMovement;
			double moveSpeed;
			double moveDrag;
			double mouseSpeed;
			double mouseDrag;
			double slowModifier;
			double fastModifier;
			double veryFastModifier;
		} camera;
	};
}
