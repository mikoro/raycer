// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Rendering/Framebuffer.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Rendering/GLHelper.h"
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

	glGenTextures(1, &textureId);
	checkGLError("Could not create OpenGL textures");

	// prevent color sampling errors on the framebuffer edges, especially when using linear filtering
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	checkGLError("Could not set OpenGL texture parameters");

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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	checkGLError("Could not set OpenGL buffer parameters");
}

void Framebuffer::resize(int width_, int height_)
{
	assert(width_ > 0 && height_ > 0);

	width = width_;
	height = height_;
	length = width * height;

	App::getLog().logInfo("Resizing framebuffer to %sx%s", width, height);

	if (pixelData != nullptr)
		_mm_free(pixelData);

	pixelData = (float*)_mm_malloc(length * sizeof(float) * 4, 64);

	if (pixelData == nullptr)
		throw std::runtime_error("Could not allocate memory for the framebuffer");

	// reserve the texture memory on the device
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	checkGLError("Could not reserve OpenGL texture memory");

	clear();
}

void Framebuffer::setPixel(int x, int y, const Color& color)
{
	assert(x < width && y < height);

	int pixelIndex = y * width * 4 + x * 4;

	pixelData[pixelIndex] = (float)color.r;
	pixelData[pixelIndex + 1] = (float)color.g;
	pixelData[pixelIndex + 2] = (float)color.b;
	pixelData[pixelIndex + 3] = (float)color.a;
}

void Framebuffer::setPixel(int index, const Color& color)
{
	assert(index < length);

	int pixelIndex = index * 4;

	pixelData[pixelIndex] = (float)color.r;
	pixelData[pixelIndex + 1] = (float)color.g;
	pixelData[pixelIndex + 2] = (float)color.b;
	pixelData[pixelIndex + 3] = (float)color.a;
}

float* Framebuffer::getPixelData() const
{
	return pixelData;
}

GLuint Framebuffer::getTextureId() const
{
	return textureId;
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
	for (int i = 0; i < length * 4; ++i)
		pixelData[i] = 0.0f;
}

void Framebuffer::clear(const Color& color)
{
	for (int i = 0; i < length; ++i)
	{
		int pixelIndex = i * 4;

		pixelData[pixelIndex] = (float)color.r;
		pixelData[pixelIndex + 1] = (float)color.g;
		pixelData[pixelIndex + 2] = (float)color.b;
		pixelData[pixelIndex + 3] = (float)color.a;
	}
}

void Framebuffer::render() const
{
	Settings& settings = App::getSettings();

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
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)width, (GLsizei)height, GL_RGBA, GL_FLOAT, pixelData);
		checkGLError("Could not upload OpenGL texture data");
	}

	glBindVertexArray(vaoId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	checkGLError("Could not render the framebuffer");
}

void Framebuffer::enableSmoothing(bool state)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, state ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, state ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	checkGLError("Could not set OpenGL texture parameters");
}
