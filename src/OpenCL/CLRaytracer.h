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

		void initialize(const Scene& scene);
		void resizeImageBuffer(int width, int height);
		void releaseImageBuffer();
		void run(RaytracerState& state, std::atomic<bool>& interrupted);
		Image downloadImage();

	private:

		CLRaytracer(const CLRaytracer& c);
		CLRaytracer& operator=(const CLRaytracer& c);

		void createBuffers();
		void uploadFullData();
		void uploadCameraData();
		void createTextureImages();
		
		CLScene clScene;

		int imageBufferWidth = 0;
		int imageBufferHeight = 0;

		cl_mem statePtr = nullptr;
		cl_mem cameraPtr = nullptr;
		cl_mem raytracerPtr = nullptr;
		cl_mem toneMapperPtr = nullptr;
		cl_mem simpleFogPtr = nullptr;
		cl_mem materialsPtr = nullptr;
		cl_mem ambientLightPtr = nullptr;
		cl_mem directionalLightsPtr = nullptr;
		cl_mem pointLightsPtr = nullptr;
		cl_mem trianglesPtr = nullptr;
		cl_mem bvhNodesPtr = nullptr;
		cl_mem outputImagePtr = nullptr;
		
		std::vector<cl_mem> textureImagePtrs;
		cl_mem dummyTextureImagePtr = nullptr;
		const int KERNEL_TEXTURE_COUNT = 60;

		cl_program raytraceProgram = nullptr;
		cl_kernel raytraceKernel = nullptr;

		int kernelArgumentIndex = 0;
		int outputImageArgumentIndex = 0;
		bool initialized = false;
	};
}
