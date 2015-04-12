// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cassert>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "Rendering/Image.h"
#include "App.h"
#include "Utils/Log.h"
#include "Rendering/Framebuffer.h"
#include "Math/Color.h"

using namespace Raycer;

Image::Image()
{
}

Image::Image(size_t length)
{
	setSize(length);
}

Image::Image(size_t width_, size_t height_)
{
	setSize(width_, height_);
}

Image::Image(const Image& image)
{
	size_t otherWidth = image.getWidth();
	size_t otherHeight = image.getHeight();
	size_t pixelCount = otherWidth * otherHeight;

	setSize(otherWidth, otherHeight);

	for (size_t i = 0; i < pixelCount; ++i)
		pixelData[i] = image.pixelData[i];
}

Image::Image(const std::string& fileName)
{
	load(fileName);
}

Image::~Image()
{
	if (pixelData != nullptr)
	{
		_mm_free(pixelData);
		pixelData = nullptr;
	}
}

Image& Image::operator=(const Image& image)
{
	size_t otherWidth = image.getWidth();
	size_t otherHeight = image.getHeight();
	size_t pixelCount = otherWidth * otherHeight;

	setSize(otherWidth, otherHeight);

	for (size_t i = 0; i < pixelCount; ++i)
		pixelData[i] = image.pixelData[i];

	return *this;
}

void Image::load(const std::string& fileName)
{
	App::getLog().logInfo("Loading image from %s", fileName);

	int newWidth, newHeight, components;
	uint32_t* imageData = (uint32_t*)stbi_load(fileName.c_str(), &newWidth, &newHeight, &components, 4);

	if (imageData == nullptr)
		throw std::runtime_error("Could not load data from the image file");

	setSize((size_t)newWidth, (size_t)newHeight);

	for (size_t i = 0; i < (size_t)(newWidth * newHeight); ++i)
		pixelData[i] = imageData[i];

	stbi_image_free(imageData);
}

namespace
{
	bool endsWith(const std::string& text, const std::string& suffix)
	{
		return text.rfind(suffix) == (text.size() - suffix.size());
	}
}

void Image::saveAs(const std::string& fileName) const
{
	App::getLog().logInfo("Saving image to %s", fileName);

	int result = 0;

	if (endsWith(fileName, ".png"))
		result = stbi_write_png(fileName.c_str(), (int)width, (int)height, 4, (uint8_t*)pixelData, (int)(width * sizeof(uint32_t)));
	else if (endsWith(fileName, ".bmp"))
		result = stbi_write_bmp(fileName.c_str(), (int)width, (int)height, 4, (uint8_t*)pixelData);
	else if (endsWith(fileName, ".tga"))
		result = stbi_write_tga(fileName.c_str(), (int)width, (int)height, 4, (uint8_t*)pixelData);
	else
		throw std::runtime_error("Could not save the image (non-supported format)");

	if (result == 0)
		throw std::runtime_error("Could not save the image");
}

void Image::setSize(size_t length)
{
	setSize(length, 1);
}

void Image::setSize(size_t width_, size_t height_)
{
	assert(width_ > 0 && height_ > 0);

	width = width_;
	height = height_;

	if (pixelData != nullptr)
		_mm_free(pixelData);

	pixelData = (uint32_t*)_mm_malloc(width * height * sizeof(uint32_t), 64);

	if (pixelData == nullptr)
		throw std::runtime_error("Could not allocate memory for the image");

	memset(pixelData, 0, width * height * sizeof(uint32_t));
}

void Image::setPixel(size_t x, size_t y, const Color& color)
{
	assert(x < width && y < height);

	pixelData[y * width + x] = color.getRgbaValue();
}

void Image::swapBytes()
{
	Image swappedImage(width, height);

	for (size_t i = 0; i < (width * height); ++i)
	{
		uint32_t rgba = pixelData[i];
		uint32_t abgr = 0;

		abgr |= (rgba << 24) & 0xff000000;
		abgr |= (rgba << 8) & 0x00ff0000;
		abgr |= (rgba >> 8) & 0x0000ff00;
		abgr |= (rgba >> 24) & 0x000000ff;

		swappedImage.pixelData[i] = abgr;
	}

	std::swap(pixelData, swappedImage.pixelData);
}

void Image::flip()
{
	Image flippedImage(width, height);

	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
			flippedImage.pixelData[(height - 1 - y) * width + x] = pixelData[y * width + x];
	}

	std::swap(pixelData, flippedImage.pixelData);
}

size_t Image::getWidth() const
{
	return width;
}

size_t Image::getHeight() const
{
	return height;
}

Color Image::getPixel(size_t x, size_t y) const
{
	assert(x < width && y < height);

	return Color::fromRgbaValue(pixelData[y * width + x]);
}

Color Image::getPixel(double s, double t) const
{
	assert(s >= 0.0 && s <= 1.0 && t >= 0.0 && t <= 1.0);

	return getPixel((size_t)(s * (double)width + 0.5), (size_t)(t * (double)height + 0.5));
}
