// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#include "OpenCL/CLScene.h"
#include "Rendering/Image.h"

namespace Raycer
{
	class Scene;
	struct RaytracerState;

	class CLRaytracer
	{
	public:

		CLRaytracer();
		~CLRaytracer();

		void initialize();
		void resizePixelsBuffer(int width, int height);
		void releasePixelsBuffer();
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
