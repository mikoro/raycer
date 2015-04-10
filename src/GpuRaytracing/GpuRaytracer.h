// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.h>

namespace Raycer
{
	class Scene;
	class Image;

	class GpuRaytracer
	{
	public:

		GpuRaytracer();
		~GpuRaytracer();

		void initialize();
		void setSize(int width, int height);
		void trace(const Scene& scene, std::atomic<bool>& interrupted, std::atomic<int>& pixelCount, std::atomic<int>& rayCount);
		Image getImage();

	private:

		GpuRaytracer(const GpuRaytracer& gpuRaytracer);
		GpuRaytracer& operator=(const GpuRaytracer& gpuRaytracer);
	};
}
