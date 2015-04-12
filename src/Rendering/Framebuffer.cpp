// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Rendering/Framebuffer.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Utils/OpenGL.h"
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
	Settings& settings = App::getSettings();

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

	programId = OpenGL::buildProgram(settings.framebuffer.vertexShader, settings.framebuffer.fragmentShader);
	samplerId = glGetUniformLocation(programId, "tex0");

	const GLfloat vertexData[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
	};

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	checkGlError("Could not set OpenGL buffer parameters");
}

void Framebuffer::setSize(size_t width_, size_t height_)
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

	// reserve the texture memory on the device
	glBindTexture(GL_TEXTURE_2D, cpuTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	checkGlError("Could not reserve OpenGL texture memory (CPU texture)");

	glBindTexture(GL_TEXTURE_2D, gpuTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	checkGlError("Could not reserve OpenGL texture memory (GPU texture)");

	clear();
}

void Framebuffer::setPixel(size_t x, size_t y, const Color& color)
{
	assert(x < width && y < height);

	pixelData[y * width + x] = color.getAbgrValue();
}

Color Framebuffer::getPixel(size_t x, size_t y) const
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

size_t Framebuffer::getWidth() const
{
	return width;
}

size_t Framebuffer::getHeight() const
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

	glUseProgram(programId);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(samplerId, 0);

	if (settings.openCL.enabled)
		glBindTexture(GL_TEXTURE_2D, gpuTextureId);
	else
	{
		glBindTexture(GL_TEXTURE_2D, cpuTextureId);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)width, (GLsizei)height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pixelData);
		checkGlError("Could not upload OpenGL texture data");
	}

	glBindVertexArray(vaoId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	
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
