// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#ifdef __APPLE__
#include <mm_malloc.h>
#endif

#include "Rendering/Framebuffer.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Rendering/GLHelper.h"
#include "Math/Color.h"

using namespace Raycer;

Framebuffer::Framebuffer()
{
}

Framebuffer::~Framebuffer()
{
	if (floatPixelData != nullptr)
	{
		_mm_free(floatPixelData);
		floatPixelData = nullptr;
	}
}

void Framebuffer::initialize()
{
	Settings& settings = App::getSettings();

	App::getLog().logInfo("Initializing framebuffer");

	glGenTextures(1, &textureId);
	GLHelper::checkError("Could not create OpenGL textures");

	// prevent color sampling errors on the framebuffer edges, especially when using linear filtering
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	GLHelper::checkError("Could not set OpenGL texture parameters");

	programId = GLHelper::buildProgram(settings.framebuffer.vertexShader, settings.framebuffer.fragmentShader);

	samplerUniformId = glGetUniformLocation(programId, "tex0");
	textureWidthUniformId = glGetUniformLocation(programId, "textureWidth");
	textureHeightUniformId = glGetUniformLocation(programId, "textureHeight");
	texelWidthUniformId = glGetUniformLocation(programId, "texelWidth");
	texelHeightUniformId = glGetUniformLocation(programId, "texelHeight");

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

	GLHelper::checkError("Could not set OpenGL buffer parameters");
}

void Framebuffer::resize(int width_, int height_)
{
	assert(width_ > 0 && height_ > 0);

	width = width_;
	height = height_;
	length = width * height;

	App::getLog().logInfo("Resizing framebuffer to %sx%s", width, height);

	image.setSize(width, height);

	if (floatPixelData != nullptr)
		_mm_free(floatPixelData);

	floatPixelData = (float*)_mm_malloc(length * sizeof(float) * 4, 64);

	if (floatPixelData == nullptr)
		throw std::runtime_error("Could not allocate memory for the framebuffer");

	// reserve the texture memory on the device
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	GLHelper::checkError("Could not reserve OpenGL texture memory");

	clear();
}

void Framebuffer::clear()
{
	image.clear();
}

void Framebuffer::clear(const Color& color)
{
	image.clear(color);
}

void Framebuffer::render() const
{
	Settings& settings = App::getSettings();

	for (int i = 0; i < length; ++i)
	{
		int pixelIndex = i * 4;

		floatPixelData[pixelIndex] = (float)image.pixelData[i].r;
		floatPixelData[pixelIndex + 1] = (float)image.pixelData[i].g;
		floatPixelData[pixelIndex + 2] = (float)image.pixelData[i].b;
		floatPixelData[pixelIndex + 3] = (float)image.pixelData[i].a;
	}

	glUseProgram(programId);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(samplerUniformId, 0);
	glUniform1f(textureWidthUniformId, (float)width);
	glUniform1f(textureHeightUniformId, (float)height);
	glUniform1f(texelWidthUniformId, 1.0f / (float)width);
	glUniform1f(texelHeightUniformId, 1.0f / (float)height);

	glBindTexture(GL_TEXTURE_2D, textureId);

	if (!settings.openCL.enabled)
	{
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)width, (GLsizei)height, GL_RGBA, GL_FLOAT, floatPixelData);
		GLHelper::checkError("Could not upload OpenGL texture data");
	}

	glBindVertexArray(vaoId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	GLHelper::checkError("Could not render the framebuffer");
}

void Framebuffer::enableSmoothing(bool state)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, state ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, state ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLHelper::checkError("Could not set OpenGL texture parameters");
}

int Framebuffer::getWidth() const
{
	return width;
}

int Framebuffer::getHeight() const
{
	return height;
}

GLuint Framebuffer::getTextureId() const
{
	return textureId;
}
