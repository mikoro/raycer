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
			int deviceId;
			int deviceType;
		} openCl;

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
			std::string infoFont;
			int infoFontSize;
		} window;

		struct Framebuffer
		{
			double scale;
			bool smoothing;
		} framebuffer;

		struct Controls
		{
			double moveSpeed;
			double mouseSpeed;
			bool freeLook;
		} controls;
	};
}
