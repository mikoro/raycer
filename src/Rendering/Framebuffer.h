// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <cstdint>

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

		void setSize(size_t width, size_t height);

		void setPixel(size_t x, size_t y, const Color& color);
		Color getPixel(size_t x, size_t y) const;

		uint32_t* getPixelData() const;
		uint32_t getCpuTextureId() const;
		uint32_t getGpuTextureId() const;
		size_t getWidth() const;
		size_t getHeight() const;

		void clear();
		void clear(const Color& color);
		void render() const;

		void enableSmoothing(bool state);

	private:

		Framebuffer(const Framebuffer& framebuffer);
		Framebuffer& operator=(const Framebuffer& framebuffer);

		size_t width = 0;
		size_t height = 0;

		uint32_t* pixelData = nullptr;

		uint32_t cpuTextureId = 0;
		uint32_t gpuTextureId = 0;
	};
}
