// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <GL/glcorearb.h>

namespace Raycer
{
	class Film;
	class Color;

	class FilmRenderer
	{
	public:

		void initialize();
		void setFilmSize(uint64_t width, uint64_t height);
		void setWindowSize(uint64_t width, uint64_t height);
		void uploadFilmData(const Film& film);
		void render();
		
		GLuint getFilmTextureId() const;

	private:

		uint64_t filmWidth = 0;
		uint64_t filmHeight = 0;
		uint64_t windowWidth = 0;
		uint64_t windowHeight = 0;

		GLuint vaoId = 0;
		GLuint vertexBufferId = 0;
		GLuint framebufferId = 0;
		GLuint filmTextureId = 0;
		GLuint fullscreenTextureId = 0;

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
