// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "Rendering/Image.h"
#include "Rendering/Framebuffer.h"
#include "Math/Color.h"

using namespace Raycer;

Image::Image()
{
}

Image::Image(const std::string& fileName)
{
	load(fileName);
}

Image::Image(const Framebuffer& framebuffer)
{
	(void)framebuffer;
}

Image::~Image()
{
	if (pixelData != nullptr)
	{
		_mm_free(pixelData);
		pixelData = nullptr;
	}
}

void Image::load(const std::string& fileName)
{

}

void Image::saveAs(const std::string& fileName) const
{

}

void Image::setSize(int width_, int height_)
{
	assert(width_ > 0 && height_ > 0);

	width = width_;
	height = height_;

	if (pixelData != nullptr)
		_mm_free(pixelData);

	pixelData = (uint32_t*)_mm_malloc(width * height * sizeof(uint32_t), 64);

	if (pixelData == nullptr)
		throw std::runtime_error("Could not allocate memory for the image");
}

void Image::setPixel(int x, int y, const Color& color)
{
	assert(x < width && y < height);

	pixelData[y * width + x] = color.getRgbaValue();
}

int Image::getWidth() const
{
	return width;
}

int Image::getHeight() const
{
	return height;
}

Color Image::getPixel(int x, int y) const
{
	assert(x < width && y < height);

	return Color::fromRgbaValue(pixelData[y * width + x]);
}

Color Image::getPixel(double s, double t) const
{
	assert(s >= 0.0 && s <= 1.0 && t >= 0.0 && t <= 1.0);

	return getPixel((int)(s * width + 0.5), (int)(t * height + 0.5));
}
