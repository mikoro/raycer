// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <memory>

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.h>

namespace Raycer
{
	class BaseLog;
	class NamedLog;
	struct ConsoleSettings;

	class OpenCLHelper
	{
	public:

		OpenCLHelper(BaseLog& baseLog);
		~OpenCLHelper();
		
		void initialize(ConsoleSettings& consoleSettings);
		void loadKernels();

		cl_kernel raytraceKernel = nullptr;

	private:

		std::unique_ptr<NamedLog> log;

		cl_platform_id platformId = nullptr;
		cl_device_id deviceId = nullptr;
		cl_context context = nullptr;
		cl_command_queue commandQueue = nullptr;
		cl_program program = nullptr;
	};
}
