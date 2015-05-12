// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <CL/opencl.h>

namespace Raycer
{
	class CLManager
	{
	public:

		CLManager();
		~CLManager();

		void initialize();
		void loadKernels();

		static void checkError(int result, const std::string& message);
		static std::string getErrorMessage(int result);

		cl_platform_id platformId = nullptr;
		cl_device_id deviceId = nullptr;
		cl_context context = nullptr;
		cl_command_queue commandQueue = nullptr;
		cl_program program = nullptr;
		cl_kernel raytraceKernel = nullptr;
		cl_kernel printStructSizesKernel = nullptr;

	private:

		CLManager(const CLManager& clManager);
		CLManager& operator=(const CLManager& clManager);
	};
}
