// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

namespace Raycer
{
	class Scene;
	class Image;

	class GpuRaytracer
	{
	public:

		void setSize(size_t width, size_t height);
		void trace(const Scene& scene, std::atomic<bool>& interrupted, std::atomic<size_t>& pixelCount, std::atomic<size_t>& rayCount);
		Image getImage();

	private:

		size_t width = 0;
		size_t height = 0;
	};
}
