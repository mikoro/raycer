// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "glfw/glfw3.h"
#include "gl/glext.h"

#include "Rendering/Framebuffer.h"
#include "Utils/Log.h"
#include "Math/Color.h"

using namespace Raycer;

Framebuffer::Framebuffer(BaseLog& baseLog)
{
	log = baseLog.getNamedLog("Framebuffer");

	log->logInfo("Initializing");

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	// prevent color sampling errors on the framebuffer edges, especially when using linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	enableSmoothing(true);
}

Framebuffer::~Framebuffer()
{
	if (pixelData != nullptr)
	{
		_mm_free(pixelData);
		pixelData = nullptr;
	}
}

void Framebuffer::setSize(int width_, int height_)
{
	assert(width_ > 0 && height_ > 0);

	width = width_;
	height = height_;

	log->logInfo("Resizing to %sx%s", width, height);

	if (pixelData != nullptr)
		_mm_free(pixelData);

	pixelData = (uint32_t*)_mm_malloc(width * height * sizeof(uint32_t), 64);

	if (pixelData == nullptr)
		throw std::runtime_error("Could not allocate memory for the framebuffer");

	// reserve the texture memory
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 0);

	clear();
}

void Framebuffer::setPixel(int x, int y, const Color& color)
{
	assert(x < width && y < height);

	pixelData[y * width + x] = color.getAbgrValue();
}

Color Framebuffer::getPixel(int x, int y) const
{
	assert(x < width && y < height);

	return Color::fromAbgrValue(pixelData[y * width + x]);
}

uint32_t* Framebuffer::getPixelData() const
{
	return pixelData;
}

void Framebuffer::clear()
{
	std::fill_n(pixelData, width * height, 0);
}

void Framebuffer::clear(const Color& color)
{
	std::fill_n(pixelData, width * height, color.getAbgrValue());
}

void Framebuffer::render() const
{
	glClearColor(1.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pixelData);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 0.0);
	glEnd();
}

int Framebuffer::getWidth() const
{
	return width;
}

int Framebuffer::getHeight() const
{
	return height;
}

void Framebuffer::enableSmoothing(bool value)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, value ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, value ? GL_LINEAR : GL_NEAREST);
}
