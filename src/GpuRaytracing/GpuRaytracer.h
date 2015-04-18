// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <CL/opencl.h>

#include "Rendering/Image.h"

namespace Raycer
{
	struct RaytracerConfig;

	class GpuRaytracer
	{
	public:

		GpuRaytracer();
		~GpuRaytracer();

		void trace(RaytracerConfig& config, std::atomic<bool>& interrupted);

		void resize(int width, int height);
		void release();
		void readImage();
		Image& getImage();

	private:

		cl_mem pixels = nullptr;

		int width = 0;
		int height = 0;

		Image image;
	};
}
