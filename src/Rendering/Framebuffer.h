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

		void setSize(int width, int height);

		void setPixel(int x, int y, const Color& color);
		Color getPixel(int x, int y) const;

		uint32_t* getPixelData() const;
		uint32_t getCpuTextureId() const;
		uint32_t getGpuTextureId() const;
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

		uint32_t* pixelData = nullptr;

		uint32_t cpuTextureId = 0;
		uint32_t gpuTextureId = 0;
	};
}
