// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "glfw/glfw3.h"
#include "gl/glext.h"

#include "Rendering/Framebuffer.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Utils/Errors.h"
#include "Math/Color.h"

using namespace Raycer;

Framebuffer::Framebuffer()
{
}

Framebuffer::~Framebuffer()
{
	if (pixelData != nullptr)
	{
		_mm_free(pixelData);
		pixelData = nullptr;
	}
}

void Framebuffer::initialize()
{
	App::getLog().logInfo("Initializing framebuffer");

	glGenTextures(1, &cpuTextureId);
	glGenTextures(1, &gpuTextureId);

	checkGlError("Could not create OpenGL textures");

	// prevent color sampling errors on the framebuffer edges, especially when using linear filtering
	glBindTexture(GL_TEXTURE_2D, cpuTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, gpuTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	checkGlError("Could not set OpenGL texture parameters");
}

void Framebuffer::setSize(int width_, int height_)
{
	assert(width_ > 0 && height_ > 0);

	width = width_;
	height = height_;

	App::getLog().logInfo("Resizing framebuffer to %sx%s", width, height);

	if (pixelData != nullptr)
		_mm_free(pixelData);

	pixelData = (uint32_t*)_mm_malloc(width * height * sizeof(uint32_t), 64);

	if (pixelData == nullptr)
		throw std::runtime_error("Could not allocate memory for the framebuffer");

	// reserve the texture memory
	glBindTexture(GL_TEXTURE_2D, cpuTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	checkGlError("Could not reserve OpenGL texture memory (CPU texture)");

	glBindTexture(GL_TEXTURE_2D, gpuTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	checkGlError("Could not reserve OpenGL texture memory (GPU texture)");

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

uint32_t Framebuffer::getCpuTextureId() const
{
	return cpuTextureId;
}

uint32_t Framebuffer::getGpuTextureId() const
{
	return gpuTextureId;
}

int Framebuffer::getWidth() const
{
	return width;
}

int Framebuffer::getHeight() const
{
	return height;
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
	Settings& settings = App::getSettings();

	glClearColor(1.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (settings.openCl.enabled)
		glBindTexture(GL_TEXTURE_2D, gpuTextureId);
	else
	{
		glBindTexture(GL_TEXTURE_2D, cpuTextureId);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pixelData);
		checkGlError("Could not upload OpenGL texture data");
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 0.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	checkGlError("Could not render the framebuffer");
}

void Framebuffer::enableSmoothing(bool state)
{
	glBindTexture(GL_TEXTURE_2D, cpuTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, state ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, state ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, gpuTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, state ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, state ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	checkGlError("Could not set OpenGL texture parameters");
}
