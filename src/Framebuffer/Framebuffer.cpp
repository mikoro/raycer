// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "glfw/glfw3.h"
#include "gl/glext.h"

#include "Framebuffer/Framebuffer.h"
#include "Utils/Log.h"
#include "Math/Color.h"

namespace
{
	const int DATA_ALIGNMENT = 64;
}

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

	SetEnableSmoothing(false);
}

Framebuffer::~Framebuffer()
{
	if (pixelData != nullptr)
	{
		_mm_free(pixelData);
		pixelData = nullptr;
	}
}

void Framebuffer::resize(int width_, int height_)
{
	width = width_;
	height = height_;

	log->logInfo("Resizing to %sx%s", width, height);

	if (pixelData != nullptr)
		_mm_free(pixelData);

	pixelData = (uint32_t*)_mm_malloc(width * height * sizeof(uint32_t), DATA_ALIGNMENT);

	// reserve the texture memory
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 0);

	clear();
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

void Framebuffer::saveAsTga(const std::string& fileName) const
{
	log->logInfo("Saving as TGA to %s", fileName);
}

int Framebuffer::getWidth() const
{
	return width;
}

int Framebuffer::getHeight() const
{
	return height;
}

uint32_t* Framebuffer::getPixelData() const
{
	return pixelData;
}

void Framebuffer::SetEnableSmoothing(bool value)
{
	enableSmoothing = value;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);

	if (enableSmoothing)
	{
		// smooth texture scaling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		// blocky texture scaling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

bool Framebuffer::GetEnableSmoothing() const
{
	return enableSmoothing;
}
