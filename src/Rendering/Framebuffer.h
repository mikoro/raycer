// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <GL/glew.h>

#include "Rendering/RenderTarget.h"

namespace Raycer
{
	class Color;

	class Framebuffer : public RenderTarget
	{
	public:

		Framebuffer();
		~Framebuffer();

		void initialize();
		void resize(int width, int height);
		void setPixel(int x, int y, const Color& color);
		void setPixel(int index, const Color& color);

		float* getPixelData() const;
		GLuint getTextureId() const;
		int getWidth() const;
		int getHeight() const;

		void clear();
		void clear(const Color& color);
		void render() const;

		void enableSmoothing(bool state);

	private:

		Framebuffer(const Framebuffer& framebuffer);
		Framebuffer& operator=(const Framebuffer& framebuffer);

		int width = 0;
		int height = 0;
		int length = 0;

		float* pixelData = nullptr;

		GLuint textureId = 0;
		GLuint programId = 0;
		GLuint vaoId = 0;
		GLuint vertexBufferId = 0;
		GLuint samplerUniformId = 0;
		GLuint textureWidthUniformId = 0;
		GLuint textureHeightUniformId = 0;
		GLuint texelWidthUniformId = 0;
		GLuint texelHeightUniformId = 0;
	};
}
