// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

#include <CL/opencl.h>

namespace Raycer
{
	class Scene;
	class Image;

	class GpuRaytracer
	{
	public:

		GpuRaytracer();
		~GpuRaytracer();

		void setSize(int width, int height);
		void trace(const Scene& scene, std::atomic<bool>& interrupted, std::atomic<int>& pixelCount, std::atomic<int>& rayCount);
		Image getImage();

	private:

		GpuRaytracer(const GpuRaytracer& gpuRaytracer);
		GpuRaytracer& operator=(const GpuRaytracer& gpuRaytracer);

		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t totalPixels = 0;

		cl_mem pixels = nullptr;
	};
}
