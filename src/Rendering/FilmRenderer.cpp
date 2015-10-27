// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/FilmRenderer.h"
#include "App.h"
#include "Settings.h"
#include "Utils/GLHelper.h"
#include "Film.h"

using namespace Raycer;

void FilmRenderer::initialize()
{
	Settings& settings = App::getSettings();

	glGenTextures(1, &filmTextureId);
	glGenTextures(1, &fullscreenTextureId);

	GLHelper::checkError("Could not create OpenGL textures");

	glBindTexture(GL_TEXTURE_2D, filmTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, fullscreenTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)));
	glBindVertexArray(0);

	GLHelper::checkError("Could not set OpenGL buffer parameters");

	setWindowSize(settings.window.width, settings.window.height);

	glGenFramebuffers(1, &framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fullscreenTextureId, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("Could not initialize OpenGL framebuffer");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLHelper::checkError("Could not set OpenGL framebuffer parameters");
}

void FilmRenderer::setFilmSize(uint64_t width, uint64_t height)
{
	filmWidth = width;
	filmHeight = height;

	// reserve the texture memory on the device
	glBindTexture(GL_TEXTURE_2D, filmTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GLsizei(filmWidth), GLsizei(filmHeight), 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLHelper::checkError("Could not reserve OpenGL texture memory");
}

void FilmRenderer::setWindowSize(uint64_t width, uint64_t height)
{
	windowWidth = width;
	windowHeight = height;

	glBindTexture(GL_TEXTURE_2D, fullscreenTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GLsizei(windowWidth), GLsizei(windowHeight), 0, GL_RGBA, GL_FLOAT, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D); // TODO: needed?
	glBindTexture(GL_TEXTURE_2D, 0);

	GLHelper::checkError("Could not reserve OpenGL texture memory");
}

void FilmRenderer::uploadFilmData(const Film& film)
{
	glBindTexture(GL_TEXTURE_2D, filmTextureId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GLsizei(filmWidth), GLsizei(filmHeight), GL_RGBA, GL_FLOAT, &film.getToneMappedImage().getPixelDataConst()[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLHelper::checkError("Could not upload OpenGL texture data");
}

void FilmRenderer::render()
{
	/* Resampling pass */

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, filmTextureId);

	glUseProgram(resampleProgramId);
	glUniform1i(resampleTextureUniformId, 0);
	glUniform1f(resampleTextureWidthUniformId, float(filmWidth));
	glUniform1f(resampleTextureHeightUniformId, float(filmHeight));
	glUniform1f(resampleTexelWidthUniformId, 1.0f / filmWidth);
	glUniform1f(resampleTexelHeightUniformId, 1.0f / filmHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

	glBindVertexArray(vaoId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	/* Filtering pass */

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fullscreenTextureId);

	glUseProgram(filterProgramId);
	glUniform1i(filterTextureUniformId, 0);
	glUniform1f(filterTextureWidthUniformId, float(windowWidth));
	glUniform1f(filterTextureHeightUniformId, float(windowHeight));
	glUniform1f(filterTexelWidthUniformId, 1.0f / windowWidth);
	glUniform1f(filterTexelHeightUniformId, 1.0f / windowHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindVertexArray(vaoId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	GLHelper::checkError("Could not render the framebuffer");
}

GLuint FilmRenderer::getFilmTextureId() const
{
	return filmTextureId;
}
