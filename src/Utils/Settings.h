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
		} general;

		struct OpenCL
		{
			bool enabled;
			int platformId;
			int deviceType;
			int deviceId;
			std::string options;
		} openCL;

		struct Scene
		{
			std::string fileName;
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
			std::string vertexShader;
			std::string fragmentShader;
		} framebuffer;

		struct Controls
		{
			double moveSpeed;
			double mouseSpeed;
			bool freeLook;
		} controls;
	};
}
