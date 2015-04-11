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
		Image(size_t width, size_t height);
		Image(const Image& image);
		Image(const Framebuffer& framebuffer);
		Image(const std::string& fileName);
		~Image();

		Image& operator=(const Image& image);

		void load(const std::string& fileName);
		void saveAs(const std::string& fileName) const;
		void setSize(size_t width, size_t height);
		void setPixel(size_t x, size_t y, const Color& color);
		void swapBytes();
		void flip();

		size_t getWidth() const;
		size_t getHeight() const;
		Color getPixel(size_t x, size_t y) const;
		Color getPixel(double s, double t) const;

	private:

		size_t width = 0;
		size_t height = 0;
		uint32_t* pixelData = nullptr; // RGBA
	};
}
