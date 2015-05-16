// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#include "CLRaytracing/CLStructs.h"
#include "CLRaytracing/CLScene.h"
#include "Rendering/Image.h"

namespace Raycer
{
	struct RaytracerState;

	class CLRaytracer
	{
	public:

		CLRaytracer();
		~CLRaytracer();

		void initialize();
		void resizePixelBuffer(int width, int height);
		void releasePixelBuffer();
		void run(RaytracerState& state, std::atomic<bool>& interrupted);
		Image& getImage();
		void printStructSizes();

	private:

		void convertSceneData(const Scene& scene);
		void uploadSceneData();

		CLRaytracer(const CLRaytracer& clRaytracer);
		CLRaytracer& operator=(const CLRaytracer& clRaytracer);

		cl_mem pixelsPtr = nullptr;
		cl_mem infoPtr = nullptr;
		cl_mem cameraPtr = nullptr;
		cl_mem lightsPtr = nullptr;
		cl_mem planesPtr = nullptr;
		cl_mem spheresPtr = nullptr;

		const int MAX_LIGHTS = 100;
		const int MAX_PLANES = 100;
		const int MAX_SPHERES = 100;

		CLScene clScene;

		int bufferWidth = 0;
		int bufferHeight = 0;

		Image image;
	};
}
