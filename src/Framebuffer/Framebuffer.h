// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace Raycer
{
	class BaseLog;
	class NamedLog;
	class Color;

	class Framebuffer
	{
	public:

		Framebuffer(BaseLog& baseLog);
		~Framebuffer();

		void resize(int width, int height);
		void clear();
		void clear(const Color& color);
		void render() const;
		void saveAsTga(const std::string& fileName) const;
		int getWidth() const;
		int getHeight() const;
		uint32_t* getPixelData() const;
		void SetEnableSmoothing(bool value);
		bool GetEnableSmoothing() const;

	private:

		std::unique_ptr<NamedLog> log;

		unsigned int textureId = 0;
		int width = 0;
		int height = 0;
		bool enableSmoothing = false;
		uint32_t* pixelData = nullptr;
	};
}
