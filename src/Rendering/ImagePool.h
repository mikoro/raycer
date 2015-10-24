// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <map>
#include <vector>

/*

ImagePool is used by ImageTextures to prevent loading the same file twice to the memory.
It will also be uploaded as is to the OpenCL device and indexed appropriately.

*/

namespace Raycer
{
	class Image;

	class ImagePool
	{
	public:

		static const Image* loadImage(const std::string& fileName, bool applyGamma);
		static size_t getImageIndex(const std::string& fileName);
		static const std::vector<Image>& getImages();
		static void clear();

	private:

		static std::map<std::string, size_t> imageIndexMap;
		static std::vector<Image> images;
		static bool initialized;

		static const size_t MAX_IMAGES = 1000;
	};
}
