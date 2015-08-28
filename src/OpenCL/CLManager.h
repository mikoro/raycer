// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

namespace Raycer
{
	class CLManager
	{
	public:

		~CLManager();

		void initialize();
		void loadKernels();

		static void checkError(int result, const std::string& message);
		static std::string getErrorMessage(int result);
		static void writeStructSizes(const std::string& fileName);

		cl_platform_id platformId = nullptr;
		cl_device_id deviceId = nullptr;
		cl_context context = nullptr;
		cl_command_queue commandQueue = nullptr;
		cl_program program = nullptr;
		cl_kernel raytraceKernel = nullptr;
		cl_kernel printSizesKernel = nullptr;
	};
}
