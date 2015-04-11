// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <CL/opencl.h>

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
		void setSize(int width, int height);

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

		int width = 0;
		int height = 0;
	};
}
