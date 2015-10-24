// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

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

Image::Image(size_t length_)
{
	resize(length_);
}

Image::Image(size_t width_, size_t height_)
{
	resize(width_, height_);
}

Image::Image(size_t width_, size_t height_, float* rgbaData)
{
	load(width_, height_, rgbaData);
}

Image::Image(const std::string& fileName)
{
	load(fileName);
}

void Image::load(size_t width_, size_t height_, float* rgbaData)
{
	resize(width_, height_);

	for (size_t i = 0; i < pixelData.size(); ++i)
	{
		size_t dataIndex = i * 4;

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

		resize(size_t(newWidth), size_t(newHeight));

		for (size_t y = 0; y < height; ++y)
		{
			for (size_t x = 0; x < width; ++x)
			{
				size_t pixelIndex = y * width + x;
				size_t dataIndex = (height - 1 - y) * width * 3 + x * 3; // flip vertically

				pixelData[pixelIndex].r = loadData[dataIndex];
				pixelData[pixelIndex].g = loadData[dataIndex + 1];
				pixelData[pixelIndex].b = loadData[dataIndex + 2];
				pixelData[pixelIndex].a = 1.0f;
			}
		}

		stbi_image_free(loadData);
	}
	else
	{
		int newWidth, newHeight, components;
		uint32_t* loadData = reinterpret_cast<uint32_t*>(stbi_load(fileName.c_str(), &newWidth, &newHeight, &components, 4)); // RGBA

		if (loadData == nullptr)
			throw std::runtime_error(tfm::format("Could not load image file: %s", stbi_failure_reason()));

		resize(size_t(newWidth), size_t(newHeight));

		for (size_t y = 0; y < height; ++y)
		{
			for (size_t x = 0; x < width; ++x)
				pixelData[y * width + x] = Color::fromAbgrValue(loadData[(height - 1 - y) * width + x]).toColorf(); // flip vertically
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

		for (size_t y = 0; y < height; ++y)
		{
			for (size_t x = 0; x < width; ++x)
				saveData[(height - 1 - y) * width + x] = pixelData[y * width + x].toColor().getAbgrValue(); // flip vertically
		}

		if (StringUtils::endsWith(fileName, ".png"))
			result = stbi_write_png(fileName.c_str(), int(width), int(height), 4, &saveData[0], int(width * sizeof(uint32_t)));
		else if (StringUtils::endsWith(fileName, ".bmp"))
			result = stbi_write_bmp(fileName.c_str(), int(width), int(height), 4, &saveData[0]);
		else if (StringUtils::endsWith(fileName, ".tga"))
			result = stbi_write_tga(fileName.c_str(), int(width), int(height), 4, &saveData[0]);
	}
	else if (StringUtils::endsWith(fileName, ".hdr"))
	{
		std::vector<float> saveData(pixelData.size() * 3);

		for (size_t y = 0; y < height; ++y)
		{
			for (size_t x = 0; x < width; ++x)
			{
				size_t dataIndex = (height - 1 - y) * width * 3 + x * 3; // flip vertically
				size_t pixelIndex = y * width + x;

				saveData[dataIndex] = pixelData[pixelIndex].r;
				saveData[dataIndex + 1] = pixelData[pixelIndex].g;
				saveData[dataIndex + 2] = pixelData[pixelIndex].b;
			}
		}

		result = stbi_write_hdr(fileName.c_str(), int(width), int(height), 3, &saveData[0]);
	}
	else
		throw std::runtime_error("Could not save the image (non-supported format)");

	if (result == 0)
		throw std::runtime_error(tfm::format("Could not save the image: %s", stbi_failure_reason()));
}

void Image::resize(size_t length_)
{
	resize(length_, 1);
}

void Image::resize(size_t width_, size_t height_)
{
	width = width_;
	height = height_;

	pixelData.resize(width * height);
	clear();
}

void Image::setPixel(size_t x, size_t y, const Color& color)
{
	pixelData[y * width + x] = color.toColorf();
}

void Image::setPixel(size_t index, const Color& color)
{
	pixelData[index] = color.toColorf();
}

void Image::clear()
{
	for (Colorf& c : pixelData)
		c = Colorf(0.0f, 0.0f, 0.0f, 1.0f);
}

void Image::clear(const Color& color)
{
	for (Colorf& c : pixelData)
		c = color.toColorf();
}

void Image::applyFastGamma(double gamma)
{
	for (Colorf& c : pixelData)
		c = Color::fastPow(c.toColor(), gamma).clamped().toColorf();
}

void Image::swapComponents()
{
	for (Colorf& c1 : pixelData)
	{
		Colorf c2 = c1;

		c1.r = c2.a;
		c1.g = c2.b;
		c1.b = c2.g;
		c1.a = c2.r;
	}
}

void Image::flip()
{
	Image tempImage(width, height);

	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
			tempImage.pixelData[(height - 1 - y) * width + x] = pixelData[y * width + x];
	}

	*this = tempImage;
}

void Image::fillTestPattern()
{
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			Color color = Color::BLACK;

			if (x % 2 == 0 && y % 2 == 0)
				color = Color::lerp(Color::RED, Color::BLUE, double(x) / double(width));

			pixelData[y * width + x] = color.toColorf();
		}
	}
}

size_t Image::getWidth() const
{
	return width;
}

size_t Image::getHeight() const
{
	return height;
}

size_t Image::getLength() const
{
	return width * height;
}

Color Image::getPixel(size_t x, size_t y) const
{
	assert(x < width && y < height);
	return pixelData[y * width + x].toColor();
}

Color Image::getPixel(size_t index) const
{
	assert(index < width * height);
	return pixelData[index].toColor();
}

Color Image::getPixelNearest(double u, double v) const
{
	size_t x = size_t(u * double(width - 1) + 0.5);
	size_t y = size_t(v * double(height - 1) + 0.5);

	return getPixel(x, y);
}

Color Image::getPixelBilinear(double u, double v) const
{
	double dx = u * double(width - 1) - 0.5;
	double dy = v * double(height - 1) - 0.5;
	size_t ix = size_t(dx);
	size_t iy = size_t(dy);
	double tx2 = dx - double(ix);
	double ty2 = dy - double(iy);
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

AlignedColorfVector& Image::getPixelData()
{
	return pixelData;
}

const AlignedColorfVector& Image::getPixelDataConst() const
{
	return pixelData;
}

std::map<std::string, size_t> ImagePool::imageIndexMap = std::map<std::string, size_t>();
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
		imageIndexMap[fileName] = images.size() - 1;

		if (applyGamma)
			images.back().applyFastGamma(2.2);
	}

	// the limit is arbitrary, increase it if it becomes a problem
	// idea is to prevent push_back from invalidating pointers
	if (images.size() > MAX_IMAGES)
		throw std::runtime_error("Image pool maximum size exceeded");

	return &images[imageIndexMap[fileName]];
}

size_t ImagePool::getImageIndex(const std::string& fileName)
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
