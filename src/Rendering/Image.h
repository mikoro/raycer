// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <map>
#include <string>
#include <vector>

#include "Math/Color.h"

/*

Origin (0, 0) is at the bottom left corner.

ImagePool is used by ImageTextures to prevent loading the same file twice to the memory.
It will also be uploaded as is to the OpenCL device and indexed appropriately.

*/

namespace Raycer
{
	class Image
	{
	public:

		Image();
		Image(int length);
		Image(int width, int height);
		Image(int width, int height, float* rgbaData);
		Image(const std::string& fileName);

		void load(int width, int height, float* rgbaData);
		void load(const std::string& fileName);
		void save(const std::string& fileName) const;
		void resize(int length);
		void resize(int width, int height);
		void setPixel(int x, int y, const Color& color);
		void setPixel(int index, const Color& color);
		void clear();
		void clear(const Color& color);
		void applyFastGamma(double gamma);
		void swapComponents();
		void flip();
		void fillTestPattern();

		int getWidth() const;
		int getHeight() const;

		Color getPixel(int x, int y) const;
		Color getPixelNearest(double u, double v) const;
		Color getPixelBilinear(double u, double v) const;

		std::vector<Color>& getPixelData();
		std::vector<float> getFloatData() const;

	private:

		int width = 0;
		int height = 0;

		std::vector<Color> pixelData;
	};

	class ImagePool
	{
	public:

		static const Image* loadImage(const std::string& fileName);
		static int getImageIndex(const std::string& fileName);
		static const std::vector<Image>& getImages();
		static void clear();

	private:

		static std::map<std::string, int> imageIndexMap;
		static std::vector<Image> images;
		static bool initialized;

		static const int MAX_IMAGES = 1000;
	};
}
