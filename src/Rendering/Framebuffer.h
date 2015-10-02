// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <GL/glcorearb.h>
#include <vector>

#include "Rendering/Image.h"

namespace Raycer
{
	class Color;

	class Framebuffer
	{
	public:

		Framebuffer();

		void initialize();
		void resize(size_t width, size_t height);
		void setWindowSize(size_t width, size_t height);
		void clear();
		void clear(const Color& color);
		void render();
		void enableSmoothing(bool state);

		size_t getWidth() const;
		size_t getHeight() const;
		Image& getImage();
		GLuint getImageTextureId() const;

	private:

		Image image;
		std::vector<float> floatPixelData;

		size_t windowWidth = 0;
		size_t windowHeight = 0;

		GLuint vaoId = 0;
		GLuint vertexBufferId = 0;
		GLuint framebufferId = 0;
		GLuint imageTextureId = 0;
		GLuint framebufferTextureId = 0;

		GLuint resampleProgramId = 0;
		GLuint resampleTextureUniformId = 0;
		GLuint resampleTextureWidthUniformId = 0;
		GLuint resampleTextureHeightUniformId = 0;
		GLuint resampleTexelWidthUniformId = 0;
		GLuint resampleTexelHeightUniformId = 0;

		GLuint filterProgramId = 0;
		GLuint filterTextureUniformId = 0;
		GLuint filterTextureWidthUniformId = 0;
		GLuint filterTextureHeightUniformId = 0;
		GLuint filterTexelWidthUniformId = 0;
		GLuint filterTexelHeightUniformId = 0;
	};
}
