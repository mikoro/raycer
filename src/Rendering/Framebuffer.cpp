// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cassert>
#include <stdexcept>

#include <GL/gl3w.h>

#include "Rendering/Framebuffer.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Utils/GLHelper.h"
#include "Math/Color.h"

using namespace Raycer;

Framebuffer::Framebuffer()
{
}

void Framebuffer::initialize()
{
	Settings& settings = App::getSettings();

	App::getLog().logInfo("Initializing framebuffer");

	glGenTextures(1, &imageTextureId);
	glGenTextures(1, &framebufferTextureId);

	GLHelper::checkError("Could not create OpenGL textures");

	glBindTexture(GL_TEXTURE_2D, imageTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, framebufferTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLHelper::checkError("Could not set OpenGL texture parameters");

	resampleProgramId = GLHelper::buildProgram(settings.framebuffer.resampleVertexShader, settings.framebuffer.resampleFragmentShader);
	filterProgramId = GLHelper::buildProgram(settings.framebuffer.filterVertexShader, settings.framebuffer.filterFragmentShader);

	resampleTextureUniformId = glGetUniformLocation(resampleProgramId, "texture0");
	resampleTextureWidthUniformId = glGetUniformLocation(resampleProgramId, "textureWidth");
	resampleTextureHeightUniformId = glGetUniformLocation(resampleProgramId, "textureHeight");
	resampleTexelWidthUniformId = glGetUniformLocation(resampleProgramId, "texelWidth");
	resampleTexelHeightUniformId = glGetUniformLocation(resampleProgramId, "texelHeight");

	filterTextureUniformId = glGetUniformLocation(filterProgramId, "texture0");
	filterTextureWidthUniformId = glGetUniformLocation(filterProgramId, "textureWidth");
	filterTextureHeightUniformId = glGetUniformLocation(filterProgramId, "textureHeight");
	filterTexelWidthUniformId = glGetUniformLocation(filterProgramId, "texelWidth");
	filterTexelHeightUniformId = glGetUniformLocation(filterProgramId, "texelHeight");

	GLHelper::checkError("Could not get GLSL uniforms");

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

	setWindowSize(settings.window.width, settings.window.height);

	glGenFramebuffers(1, &framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTextureId, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("Could not initialize OpenGL framebuffer");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLHelper::checkError("Could not set OpenGL framebuffer parameters");
}

void Framebuffer::resize(int width, int height)
{
	assert(width > 0 && height > 0);

	App::getLog().logInfo("Resizing framebuffer to %sx%s", width, height);

	image.resize(width, height);
	floatPixelData.resize(width * height * sizeof(float) * 4);

	// reserve the texture memory on the device
	glBindTexture(GL_TEXTURE_2D, imageTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLHelper::checkError("Could not reserve OpenGL texture memory");

	clear();
}

void Framebuffer::setWindowSize(int width, int height)
{
	assert(width > 0 && height > 0);

	windowWidth = width;
	windowHeight = height;

	glBindTexture(GL_TEXTURE_2D, framebufferTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)windowWidth, (GLsizei)windowHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::clear()
{
	image.clear();
}

void Framebuffer::clear(const Color& color)
{
	image.clear(color);
}

void Framebuffer::render()
{
	Settings& settings = App::getSettings();
	
	std::vector<Color>& imagePixelData = image.getPixelData();

	// convert image data from Color to float array
	for (int i = 0; i < (int)imagePixelData.size(); ++i)
	{
		int pixelIndex = i * 4;

		floatPixelData[pixelIndex] = (float)imagePixelData[i].r;
		floatPixelData[pixelIndex + 1] = (float)imagePixelData[i].g;
		floatPixelData[pixelIndex + 2] = (float)imagePixelData[i].b;
		floatPixelData[pixelIndex + 3] = (float)imagePixelData[i].a;
	}

	int imageWidth = image.getWidth();
	int imageHeight = image.getHeight();

	/* Resampling pass */

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, imageTextureId);

	glUseProgram(resampleProgramId);
	glUniform1i(resampleTextureUniformId, 0);
	glUniform1f(resampleTextureWidthUniformId, (float)imageWidth);
	glUniform1f(resampleTextureHeightUniformId, (float)imageHeight);
	glUniform1f(resampleTexelWidthUniformId, 1.0f / (float)imageWidth);
	glUniform1f(resampleTexelHeightUniformId, 1.0f / (float)imageHeight);

	if (!settings.openCL.enabled)
	{
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)imageWidth, (GLsizei)imageHeight, GL_RGBA, GL_FLOAT, &floatPixelData[0]);
		GLHelper::checkError("Could not upload OpenGL texture data");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

	glBindVertexArray(vaoId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	/* Filtering pass */

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebufferTextureId);

	glUseProgram(filterProgramId);
	glUniform1i(filterTextureUniformId, 0);
	glUniform1f(filterTextureWidthUniformId, (float)windowWidth);
	glUniform1f(filterTextureHeightUniformId, (float)windowHeight);
	glUniform1f(filterTexelWidthUniformId, 1.0f / (float)windowWidth);
	glUniform1f(filterTexelHeightUniformId, 1.0f / (float)windowHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindVertexArray(vaoId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	GLHelper::checkError("Could not render the framebuffer");
}

void Framebuffer::enableSmoothing(bool state)
{
	glBindTexture(GL_TEXTURE_2D, imageTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, state ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, state ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, framebufferTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, state ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, state ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLHelper::checkError("Could not set OpenGL texture parameters");
}

int Framebuffer::getWidth() const
{
	return image.getWidth();
}

int Framebuffer::getHeight() const
{
	return image.getHeight();
}

Image& Framebuffer::getImage()
{
	return image;
}

GLuint Framebuffer::getImageTextureId() const
{
	return imageTextureId;
}
