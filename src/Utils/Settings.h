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

		struct Scene
		{
			std::string fileName;
		} scene;

		struct Image
		{
			int width;
			int height;
			std::string fileName;
			bool autoView;
		} image;

		struct General
		{
			bool interactive;
			bool useOpenCL;
			int openCLPlatform;
			int openCLDevice;
		} general;

		struct Window
		{
			int width;
			int height;
			bool fullscreen;
			bool vsync;
			bool hideCursor;
		} window;

		struct Framebuffer
		{
			double scale;
			bool smoothing;
		} framebuffer;

		struct Interactive
		{
			bool showFps;
			bool showCameraInfo;
			std::string infoFont;
			int infoFontSize;
		} interactive;

		struct Controls
		{
			double moveSpeed;
			double mouseSpeed;
			bool freeLook;
		} controls;
	};
}
