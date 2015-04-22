// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <CL/opencl.h>

#include "GpuRaytracing/Structs.h"
#include "GpuRaytracing/GpuScene.h"
#include "Utils/Image.h"

namespace Raycer
{
	class Scene;

	class GpuRaytracer
	{
	public:

		GpuRaytracer();
		~GpuRaytracer();

		void initialize();
		void resizePixelBuffer(int width, int height);
		void releasePixelBuffer();
		void readScene(const Scene& scene);
		void uploadData();
		void trace(std::atomic<bool>& interrupted);
		void downloadImage();
		Image& getImage();

		void printSizes();

	private:

		GpuRaytracer(const GpuRaytracer& gpuRaytracer);
		GpuRaytracer& operator=(const GpuRaytracer& gpuRaytracer);

		cl_mem pixelsPtr = nullptr;
		cl_mem infoPtr = nullptr;
		cl_mem cameraPtr = nullptr;
		cl_mem lightsPtr = nullptr;
		cl_mem planesPtr = nullptr;
		cl_mem spheresPtr = nullptr;

		const int MAX_LIGHTS = 100;
		const int MAX_PLANES = 100;
		const int MAX_SPHERES = 100;

		GpuScene gpuScene;

		int bufferWidth = 0;
		int bufferHeight = 0;

		Image image;
	};
}
