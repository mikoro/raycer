// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <cstdint>
#include <memory>

#include "Rendering/RenderTarget.h"

namespace Raycer
{
	class BaseLog;
	class NamedLog;
	class Color;

	class Framebuffer : public RenderTarget
	{
	public:

		Framebuffer(BaseLog& baseLog);
		~Framebuffer();

		void setSize(int width, int height);
		void setPixel(int x, int y, const Color& color);
		Color getPixel(int x, int y) const;

		void clear();
		void clear(const Color& color);
		void render() const;

		int getWidth() const;
		int getHeight() const;

		void enableSmoothing(bool value);

	private:

		std::unique_ptr<NamedLog> log;

		int width = 0;
		int height = 0;

		uint32_t* pixelData = nullptr;

		unsigned int textureId = 0;
	};
}
