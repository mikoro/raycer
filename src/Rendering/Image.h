// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <cstdint>
#include <string>

#include "Rendering/RenderTarget.h"

namespace Raycer
{
	class Color;

	class Image : public RenderTarget
	{
	public:

		Image();
		Image(const std::string& fileName);
		~Image();

		void load(const std::string& fileName);
		void saveAs(const std::string& fileName) const;

		void setSize(int width, int height);
		void setPixel(int x, int y, const Color& color);

		int getWidth() const;
		int getHeight() const;

		Color getPixel(int x, int y) const;
		Color getPixel(double s, double t) const;

	private:

		int width = 0;
		int height = 0;
		uint32_t* pixelData = nullptr;
	};
}
