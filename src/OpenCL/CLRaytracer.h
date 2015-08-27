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

		~CLRaytracer();

		void initialize();
		void resizeImageBuffer(int width, int height);
		void releaseImageBuffer();
		void run(RaytracerState& state, std::atomic<bool>& interrupted);
		Image downloadImage();

	private:

		void readScene(const Scene& scene);
		void createBuffers();
		void uploadData();

		CLScene clScene;
		bool buffersCreated = false;

		int imageBufferWidth = 0;
		int imageBufferHeight = 0;

		cl_mem imagePtr = nullptr;
		cl_mem statePtr = nullptr;
		cl_mem cameraPtr = nullptr;
		cl_mem raytracerPtr = nullptr;
		cl_mem toneMapperPtr = nullptr;
		cl_mem simpleFogPtr = nullptr;
		cl_mem materialsPtr = nullptr;
		cl_mem ambientLightPtr = nullptr;
		cl_mem directionalLightsPtr = nullptr;
		cl_mem pointLightsPtr = nullptr;
		cl_mem spotLightsPtr = nullptr;
		cl_mem planesPtr = nullptr;
		cl_mem spheresPtr = nullptr;
		cl_mem boxesPtr = nullptr;
		cl_mem trianglesPtr = nullptr;
	};
}
