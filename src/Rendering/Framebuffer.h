// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <GL/gl3w.h>

#include "Rendering/Image.h"

namespace Raycer
{
	class Color;

	class Framebuffer
	{
	public:

		Framebuffer();
		~Framebuffer();

		void initialize();
		void resizeImage(int imageWidth, int imageHeight);
		void resizeWindow(int windowWidth, int windowHeight);
		void clear();
		void clear(const Color& color);
		void render() const;
		void enableSmoothing(bool state);

		int getImageWidth() const;
		int getImageHeight() const;
		GLuint getImageTextureId() const;
		
		Image image;

	private:

		Framebuffer(const Framebuffer& framebuffer);
		Framebuffer& operator=(const Framebuffer& framebuffer);

		int imageWidth = 0;
		int imageHeight = 0;
		int windowWidth = 0;
		int windowHeight = 0;

		float* floatPixelData = nullptr;

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
