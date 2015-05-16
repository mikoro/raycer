// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cassert>
#include <algorithm>
#include <vector>
#include <mm_malloc.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "Rendering/Image.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/StringUtils.h"
#include "Math/Color.h"
#include "Math/MathUtils.h"

using namespace Raycer;

Image::Image()
{
}

Image::Image(int length_)
{
	setSize(length_);
}

Image::Image(int width_, int height_)
{
	setSize(width_, height_);
}

Image::Image(int width_, int height_, float* rgbaData)
{
	load(width_, height_, rgbaData);
}

Image::Image(const Image& other)
{
	setSize(other.width, other.height);

	for (int i = 0; i < length; ++i)
		pixelData[i] = other.pixelData[i];
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

Image& Image::operator=(const Image& other)
{
	setSize(other.width, other.height);

	for (int i = 0; i < length; ++i)
		pixelData[i] = other.pixelData[i];

	return *this;
}

void Image::load(int width_, int height_, float* rgbaData)
{
	setSize(width_, height_);

	for (int i = 0; i < length; ++i)
	{
		int dataIndex = i * 4;

		pixelData[i].r = rgbaData[dataIndex];
		pixelData[i].g = rgbaData[dataIndex + 1];
		pixelData[i].b = rgbaData[dataIndex + 2];
		pixelData[i].a = rgbaData[dataIndex + 3];
	}
}

void Image::load(const std::string& fileName)
{
	App::getLog().logInfo("Loading image from %s", fileName);

	if (stbi_is_hdr(fileName.c_str()))
	{
		int newWidth, newHeight, components;
		float* data = stbi_loadf(fileName.c_str(), &newWidth, &newHeight, &components, 3); // RGB

		if (data == nullptr)
			throw std::runtime_error(tfm::format("Could not load HDR image file: %s", stbi_failure_reason()));

		setSize(newWidth, newHeight);

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				int pixelIndex = y * width + x;
				int dataIndex = (height - 1 - y) * width * 3 + x * 3; // flip vertically
				
				pixelData[pixelIndex].r = data[dataIndex];
				pixelData[pixelIndex].g = data[dataIndex + 1];
				pixelData[pixelIndex].b = data[dataIndex + 2];
				pixelData[pixelIndex].a = 1.0;
			}
		}

		stbi_image_free(data);
	}
	else
	{
		int newWidth, newHeight, components;
		uint32_t* data = (uint32_t*)stbi_load(fileName.c_str(), &newWidth, &newHeight, &components, 4); // RGBA

		if (data == nullptr)
			throw std::runtime_error(tfm::format("Could not load image file: %s", stbi_failure_reason()));

		setSize(newWidth, newHeight);

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
				pixelData[y * width + x] = Color::fromAbgrValue(data[(height - 1 - y) * width + x]); // flip vertically
		}
			
		stbi_image_free(data);
	}
}

void Image::saveAs(const std::string& fileName) const
{
	App::getLog().logInfo("Saving image to %s", fileName);

	int result = 0;

	if (StringUtils::endsWith(fileName, ".png") || StringUtils::endsWith(fileName, ".bmp") || StringUtils::endsWith(fileName, ".tga"))
	{
		std::vector<uint32_t> data(length);

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
				data[(height - 1 - y) * width + x] = pixelData[y * width + x].getAbgrValue(); // flip vertically
		}

		if (StringUtils::endsWith(fileName, ".png"))
			result = stbi_write_png(fileName.c_str(), width, height, 4, &data[0], width * sizeof(uint32_t));
		else if (StringUtils::endsWith(fileName, ".bmp"))
			result = stbi_write_bmp(fileName.c_str(), width, height, 4, &data[0]);
		else if (StringUtils::endsWith(fileName, ".tga"))
			result = stbi_write_tga(fileName.c_str(), width, height, 4, &data[0]);
	}
	else if (StringUtils::endsWith(fileName, ".hdr"))
	{
		std::vector<float> data(length * 3);

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				int dataIndex = (height - 1 - y) * width * 3 + x * 3; // flip vertically
				int pixelIndex = y * width + x;

				data[dataIndex] = (float)pixelData[pixelIndex].r;
				data[dataIndex + 1] = (float)pixelData[pixelIndex].g;
				data[dataIndex + 2] = (float)pixelData[pixelIndex].b;
			}
		}

		result = stbi_write_hdr(fileName.c_str(), width, height, 3, &data[0]);
	}
	else
		throw std::runtime_error("Could not save the image (non-supported format)");

	if (result == 0)
		throw std::runtime_error(tfm::format("Could not save the image: %s", stbi_failure_reason()));
}

void Image::setSize(int length_)
{
	setSize(length_, 1);
}

void Image::setSize(int width_, int height_)
{
	width = width_;
	height = height_;
	length = width * height;

	if (pixelData != nullptr)
	{
		_mm_free(pixelData);
		pixelData = nullptr;
	}

	// allow zero length images
	if (length == 0)
		return;

	pixelData = (Color*)_mm_malloc(length * sizeof(Color), 64);

	if (pixelData == nullptr)
		throw std::runtime_error("Could not allocate memory for the image");

	clear();
}

void Image::setPixel(int x, int y, const Color& color)
{
	assert(x < width && y < height);

	pixelData[y * width + x] = color;
}

void Image::setPixel(int index, const Color& color)
{
	assert(index < length);

	pixelData[index] = color;
}

void Image::clear()
{
	for (int i = 0; i < length; ++i)
		pixelData[i] = Color(0.0, 0.0, 0.0, 1.0);
}

void Image::clear(const Color& color)
{
	for (int i = 0; i < length; ++i)
		pixelData[i] = color;
}

void Image::applyGamma(double gamma)
{
	for (int i = 0; i < length; ++i)
		pixelData[i] = Color::pow(pixelData[i], gamma);
}

void Image::swapComponents()
{
	for (int i = 0; i < length; ++i)
	{
		Color color = pixelData[i];

		pixelData[i].r = color.a;
		pixelData[i].g = color.b;
		pixelData[i].b = color.g;
		pixelData[i].a = color.r;
	}
}

void Image::flip()
{
	Image tempImage(width, height);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
			tempImage.pixelData[(height - 1 - y) * width + x] = pixelData[y * width + x];
	}

	std::swap(pixelData, tempImage.pixelData);
}

int Image::getWidth() const
{
	return width;
}

int Image::getHeight() const
{
	return height;
}

int Image::getLength() const
{
	return length;
}

Color Image::getPixel(int x, int y) const
{
	assert(x < width && y < height);

	return pixelData[y * width + x];
}

Color Image::getPixelNearest(double u, double v) const
{
	assert(u >= 0.0 && u <= 1.0 && v >= 0.0 && v <= 1.0);

	int x = (int)(u * (double)(width - 1) + 0.5);
	int y = (int)(v * (double)(height - 1) + 0.5);

	return getPixel(x, y);
}

Color Image::getPixelLinear(double u, double v) const
{
	assert(u >= 0.0 && u <= 1.0 && v >= 0.0 && v <= 1.0);

	double dx = u * (double)(width - 1) - 0.5;
	double dy = v * (double)(height - 1) - 0.5;
	int ix = (int)dx;
	int iy = (int)dy;
	double tx2 = dx - (double)ix;
	double ty2 = dy - (double)iy;
	tx2 = MathUtils::smoothstep(tx2);
	ty2 = MathUtils::smoothstep(ty2);
	double tx1 = 1.0 - tx2;
	double ty1 = 1.0 - ty2;

	Color c11 = getPixel(ix, iy);
	Color c21 = getPixel(ix + 1, iy);
	Color c12 = getPixel(ix, iy + 1);
	Color c22 = getPixel(ix + 1, iy + 1);

	return (tx1 * c11 + tx2 * c21) * ty1 + (tx1 * c12 + tx2 * c22) * ty2; // bilinear interpolation
}
