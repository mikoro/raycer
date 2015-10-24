// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

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

		cl_program createProgram(const std::vector<std::string>& sourceFilePaths);
		cl_kernel createKernel(cl_program program, const std::string& kernelName);

		static void checkError(cl_int result, const std::string& message);
		static std::string getErrorMessage(cl_int result);
		static void writeStructSizes(const std::string& fileName);

		cl_platform_id platformId = nullptr;
		cl_device_id deviceId = nullptr;
		cl_context context = nullptr;
		cl_command_queue commandQueue = nullptr;
		cl_program printSizesProgram = nullptr;
		cl_kernel printSizesKernel = nullptr;
	};
}
