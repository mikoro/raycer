// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <CL/opencl.h>

#include "Rendering/Image.h"

namespace Raycer
{
	class OpenCL
	{
	public:

		OpenCL();
		~OpenCL();

		void initialize();
		void loadKernels();
		void releaseMemoryObjects();
		void resizeBuffers(size_t width, size_t height);
		void readBufferImage();
		Image& getBufferImage();

		cl_platform_id platformId = nullptr;
		cl_device_id deviceId = nullptr;
		cl_context context = nullptr;
		cl_command_queue commandQueue = nullptr;
		cl_program program = nullptr;
		cl_mem pixels = nullptr;
		cl_kernel raytraceKernel = nullptr;

	private:

		OpenCL(const OpenCL& openCL);
		OpenCL& operator=(const OpenCL& openCL);

		Image bufferImage;
	};
}
