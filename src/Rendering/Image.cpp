// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cassert>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "tinyformat/tinyformat.h"

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
	resize(length_);
}

Image::Image(int width_, int height_)
{
	resize(width_, height_);
}

Image::Image(int width_, int height_, float* rgbaData)
{
	load(width_, height_, rgbaData);
}

Image::Image(const std::string& fileName)
{
	load(fileName);
}

void Image::load(int width_, int height_, float* rgbaData)
{
	resize(width_, height_);

	for (int i = 0; i < (int)pixelData.size(); ++i)
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
		float* loadData = stbi_loadf(fileName.c_str(), &newWidth, &newHeight, &components, 3); // RGB

		if (loadData == nullptr)
			throw std::runtime_error(tfm::format("Could not load HDR image file: %s", stbi_failure_reason()));

		resize(newWidth, newHeight);

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				int pixelIndex = y * width + x;
				int dataIndex = (height - 1 - y) * width * 3 + x * 3; // flip vertically

				pixelData[pixelIndex].r = loadData[dataIndex];
				pixelData[pixelIndex].g = loadData[dataIndex + 1];
				pixelData[pixelIndex].b = loadData[dataIndex + 2];
				pixelData[pixelIndex].a = 1.0;
			}
		}

		stbi_image_free(loadData);
	}
	else
	{
		int newWidth, newHeight, components;
		uint32_t* loadData = (uint32_t*)stbi_load(fileName.c_str(), &newWidth, &newHeight, &components, 4); // RGBA

		if (loadData == nullptr)
			throw std::runtime_error(tfm::format("Could not load image file: %s", stbi_failure_reason()));

		resize(newWidth, newHeight);

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
				pixelData[y * width + x] = Color::fromAbgrValue(loadData[(height - 1 - y) * width + x]); // flip vertically
		}

		stbi_image_free(loadData);
	}
}

void Image::save(const std::string& fileName) const
{
	App::getLog().logInfo("Saving image to %s", fileName);

	int result = 0;

	if (StringUtils::endsWith(fileName, ".png") || StringUtils::endsWith(fileName, ".bmp") || StringUtils::endsWith(fileName, ".tga"))
	{
		std::vector<uint32_t> saveData(pixelData.size());

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
				saveData[(height - 1 - y) * width + x] = pixelData[y * width + x].getAbgrValue(); // flip vertically
		}

		if (StringUtils::endsWith(fileName, ".png"))
			result = stbi_write_png(fileName.c_str(), width, height, 4, &saveData[0], width * sizeof(uint32_t));
		else if (StringUtils::endsWith(fileName, ".bmp"))
			result = stbi_write_bmp(fileName.c_str(), width, height, 4, &saveData[0]);
		else if (StringUtils::endsWith(fileName, ".tga"))
			result = stbi_write_tga(fileName.c_str(), width, height, 4, &saveData[0]);
	}
	else if (StringUtils::endsWith(fileName, ".hdr"))
	{
		std::vector<float> saveData(pixelData.size() * 3);

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				int dataIndex = (height - 1 - y) * width * 3 + x * 3; // flip vertically
				int pixelIndex = y * width + x;

				saveData[dataIndex] = (float)pixelData[pixelIndex].r;
				saveData[dataIndex + 1] = (float)pixelData[pixelIndex].g;
				saveData[dataIndex + 2] = (float)pixelData[pixelIndex].b;
			}
		}

		result = stbi_write_hdr(fileName.c_str(), width, height, 3, &saveData[0]);
	}
	else
		throw std::runtime_error("Could not save the image (non-supported format)");

	if (result == 0)
		throw std::runtime_error(tfm::format("Could not save the image: %s", stbi_failure_reason()));
}

void Image::resize(int length_)
{
	resize(length_, 1);
}

void Image::resize(int width_, int height_)
{
	width = width_;
	height = height_;

	pixelData.resize(width * height);
	clear();
}

void Image::setPixel(int x, int y, const Color& color)
{
	pixelData[y * width + x] = color;
}

void Image::setPixel(int index, const Color& color)
{
	pixelData[index] = color;
}

void Image::clear()
{
	for (int i = 0; i < (int)pixelData.size(); ++i)
		pixelData[i] = Color(0.0, 0.0, 0.0, 1.0);
}

void Image::clear(const Color& color)
{
	for (int i = 0; i < (int)pixelData.size(); ++i)
		pixelData[i] = color;
}

void Image::applyFastGamma(double gamma)
{
	for (int i = 0; i < (int)pixelData.size(); ++i)
		pixelData[i] = Color::fastPow(pixelData[i], gamma).clamped();
}

void Image::swapComponents()
{
	for (int i = 0; i < (int)pixelData.size(); ++i)
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

	*this = tempImage;
}

void Image::fillTestPattern()
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Color color = Color::BLACK;

			if (x % 2 == 0 && y % 2 == 0)
				color = Color::lerp(Color::RED, Color::BLUE, (double)x / (double)width);

			pixelData[y * width + x] = color;
		}
	}
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
	assert(x >= 0 && x < width && y >= 0 && y < height);

	return pixelData[y * width + x];
}

Color Image::getPixelNearest(double u, double v) const
{
	int x = (int)(u * (double)(width - 1) + 0.5);
	int y = (int)(v * (double)(height - 1) + 0.5);

	return getPixel(x, y);
}

Color Image::getPixelBilinear(double u, double v) const
{
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

	// bilinear interpolation
	return (tx1 * c11 + tx2 * c21) * ty1 + (tx1 * c12 + tx2 * c22) * ty2;
}

std::vector<Color>& Image::getPixelData()
{
	return pixelData;
}

std::vector<float> Image::getFloatData() const
{
	std::vector<float> floatPixelData(width * height * 4);

	for (int i = 0; i < (int)pixelData.size(); ++i)
	{
		int pixelIndex = i * 4;

		floatPixelData[pixelIndex] = (float)pixelData[i].r;
		floatPixelData[pixelIndex + 1] = (float)pixelData[i].g;
		floatPixelData[pixelIndex + 2] = (float)pixelData[i].b;
		floatPixelData[pixelIndex + 3] = (float)pixelData[i].a;
	}

	return floatPixelData;
}

std::map<std::string, int> ImagePool::imageIndexMap = std::map<std::string, int>();
std::vector<Image> ImagePool::images = std::vector<Image>();
bool ImagePool::initialized = false;

const Image* ImagePool::loadImage(const std::string& fileName, bool applyGamma)
{
	if (!initialized)
	{
		images.reserve(MAX_IMAGES);
		initialized = true;
	}

	if (!imageIndexMap.count(fileName))
	{
		images.push_back(Image(fileName));
		imageIndexMap[fileName] = (int)images.size() - 1;

		if (applyGamma)
			images.back().applyFastGamma(2.2);
	}

	// the limit is arbitrary, increase it if it becomes a problem
	// idea is to prevent push_back from invalidating pointers
	if (images.size() > MAX_IMAGES)
		throw std::runtime_error("Image pool maximum size exceeded");

	return &images[imageIndexMap[fileName]];
}

int ImagePool::getImageIndex(const std::string& fileName)
{
	return imageIndexMap[fileName];
}

const std::vector<Image>& ImagePool::getImages()
{
	return images;
}

void ImagePool::clear()
{
	imageIndexMap.clear();
	images.clear();
}
