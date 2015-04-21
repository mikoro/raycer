// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <cstdint>
#include <string>

#include "Rendering/RenderTarget.h"

namespace Raycer
{
	class Color;
	class Framebuffer;

	class Image : public RenderTarget
	{
	public:

		Image();
		Image(int length);
		Image(int width, int height);
		Image(const Image& image);
		Image(const std::string& fileName);
		~Image();

		Image& operator=(const Image& image);

		void load(const std::string& fileName);
		void saveAs(const std::string& fileName) const;
		void setSize(int length);
		void setSize(int width, int height);
		void setPixel(int x, int y, const Color& color);
		void setPixel(int index, const Color& color);
		void swapBytes();
		void flip();

		int getWidth() const;
		int getHeight() const;
		int getLength() const;
		Color getPixel(int x, int y) const;
		Color getPixel(double u, double v) const;
		uint32_t* getPixelData() const;

	private:

		uint32_t* pixelData = nullptr; // RGBA

		int width = 0;
		int height = 0;
		int length = 0;
	};
}
