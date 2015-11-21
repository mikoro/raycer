// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <random>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#include "OpenCL/CLScene.h"

namespace Raycer
{
	class Scene;
	struct TracerState;
	class Image;

	class CLTracer
	{
	public:

		CLTracer();
		~CLTracer();

		void initializeKernels();
		void releaseKernels();

		void initializeImageBuffers(uint64_t width, uint64_t height, uint64_t imageTextureId);
		void releaseImageBuffers();

		void initializeBuffers(const Scene& scene);
		void releaseBuffers();

		void run(TracerState& state, std::atomic<bool>& interrupted);
		Image downloadImage();

	private:

		void createBuffers();
		void uploadFullData();
		void uploadMinimalData();
		void createTextureImages();

		CLScene clScene;

		uint64_t imageWidth = 0;
		uint64_t imageHeight = 0;
		uint64_t imageLength = 0;

		cl_mem statePtr = nullptr;
		cl_mem generalPtr = nullptr;
		cl_mem cameraPtr = nullptr;
		cl_mem toneMapperPtr = nullptr;
		cl_mem simpleFogPtr = nullptr;
		cl_mem materialsPtr = nullptr;
		cl_mem ambientLightPtr = nullptr;
		cl_mem directionalLightsPtr = nullptr;
		cl_mem pointLightsPtr = nullptr;
		cl_mem trianglesPtr = nullptr;
		cl_mem bvhNodesPtr = nullptr;
		cl_mem seedsPtr = nullptr;
		cl_mem cumulativeColorsPtr = nullptr;
		cl_mem filterWeightsPtr = nullptr;
		cl_mem outputImagePtr = nullptr;

		std::vector<cl_mem> textureImagePtrs;
		cl_mem dummyTextureImagePtr = nullptr;
		const uint64_t KERNEL_TEXTURE_COUNT = 60;

		cl_program program = nullptr;
		cl_kernel raytraceKernel = nullptr;
		cl_kernel pathtraceKernel = nullptr;
		cl_kernel postprocessKernel = nullptr;

		uint64_t currentKernelArgumentIndex = 0;
		uint64_t seedsArgumentIndex = 0;
		uint64_t cumulativeColorsArgumentIndex = 0;
		uint64_t filterWeightsArgumentIndex = 0;

		bool buffersInitialized = false;

		std::mt19937_64 generator;
		std::vector<uint64_t> seeds;
	};
}
