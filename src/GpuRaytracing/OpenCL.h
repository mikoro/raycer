// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.h>

namespace Raycer
{
	class OpenCL
	{
	public:

		OpenCL();
		~OpenCL();
		
		void initialize();
		void loadKernels();

		cl_kernel raytraceKernel = nullptr;

	private:

		OpenCL(const OpenCL& openCL);
		OpenCL& operator=(const OpenCL& openCL);

		cl_platform_id platformId = nullptr;
		cl_device_id deviceId = nullptr;
		cl_context context = nullptr;
		cl_command_queue commandQueue = nullptr;
		cl_program program = nullptr;
	};
}
