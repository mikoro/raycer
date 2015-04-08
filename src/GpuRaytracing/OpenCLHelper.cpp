// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>
#include <fstream>
#include <string>

#include "GpuRaytracing/OpenCLHelper.h"
#include "Utils/Log.h"
#include "Runners/ConsoleSettings.h"

using namespace Raycer;

namespace
{
	NamedLog* staticLog = nullptr;

	void CL_CALLBACK openClErrorCallback(const char* message, const void* privateInfo, size_t cb, void* userData)
	{
		(void)privateInfo;
		(void)cb;
		(void)userData;

		if (staticLog != nullptr)
			staticLog->logError("OpenCL error: %s", message);
	}
}

OpenCLHelper::OpenCLHelper(BaseLog& baseLog)
{
	log = baseLog.getNamedLog("OpenCLHelper");
	staticLog = log.get();
}

OpenCLHelper::~OpenCLHelper()
{
	if (kernel != nullptr)
	{
		clReleaseKernel(kernel);
		kernel = nullptr;
	}

	if (program != nullptr)
	{
		clReleaseProgram(program);
		program = nullptr;
	}

	if (commandQueue != nullptr)
	{
		clReleaseCommandQueue(commandQueue);
		commandQueue = nullptr;
	}

	if (context != nullptr)
	{
		clReleaseContext(context);
		context = nullptr;
	}
}

void OpenCLHelper::initialize(ConsoleSettings& consoleSettings)
{
	consoleSettings.platformId = 1;

	log->logInfo("Initializing OpenCL");

	cl_uint platformCount = 0;
	cl_int status = clGetPlatformIDs(0, NULL, &platformCount);

	if (status != CL_SUCCESS || platformCount == 0)
		throw std::runtime_error("Could not find any OpenCL platforms");

	if (consoleSettings.platformId > (int)platformCount - 1)
		throw std::runtime_error("Invalid OpenCL platform id");

	cl_platform_id* platformIds = new cl_platform_id[platformCount];
	status = clGetPlatformIDs(platformCount, platformIds, NULL);
	platformId = platformIds[consoleSettings.platformId];
	delete platformIds;

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not get OpenCL platform info");

	size_t textLength = 0;
	clGetPlatformInfo(platformId, CL_PLATFORM_NAME, 0, NULL, &textLength);
	char* platformName = new char[textLength];
	clGetPlatformInfo(platformId, CL_PLATFORM_NAME, textLength, platformName, NULL);

	clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, 0, NULL, &textLength);
	char* platformVersion = new char[textLength];
	clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, textLength, platformVersion, NULL);

	log->logInfo("OpenCL platform: %s", platformName);
	log->logInfo("OpenCL version: %s", platformVersion);

	delete platformName;
	delete platformVersion;

	cl_uint deviceCount = 0;
	status = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, 0, NULL, &deviceCount);

	if (status != CL_SUCCESS || deviceCount == 0)
		throw std::runtime_error("Could not find any OpenCL GPU devices");

	if (consoleSettings.deviceId > (int)deviceCount - 1)
		throw std::runtime_error("Invalid OpenCL device id");

	cl_device_id* deviceIds = new cl_device_id[deviceCount];
	status = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, deviceCount, deviceIds, NULL);
	deviceId = deviceIds[consoleSettings.deviceId];
	delete deviceIds;

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not get OpenCL GPU device info");

	clGetDeviceInfo(deviceId, CL_DEVICE_NAME, 0, NULL, &textLength);
	char* deviceName = new char[textLength];
	clGetDeviceInfo(deviceId, CL_DEVICE_NAME, textLength, deviceName, NULL);

	log->logInfo("OpenCL device: %s", deviceName);

	delete deviceName;

	context = clCreateContext(NULL, 1, &deviceId, openClErrorCallback, NULL, &status);

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not create OpenCL device context");

	commandQueue = clCreateCommandQueue(context, deviceId, 0, &status);

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not create OpenCL command queue");

	log->logInfo("Compiling OpenCL kernel files");

	std::ifstream file("data/kernels/test.cl");
	std::stringstream ss;
	ss << file.rdbuf();
	std::string fileString = ss.str();
	const char* fileStringPtr = fileString.c_str();

	program = clCreateProgramWithSource(context, 1, &fileStringPtr, NULL, &status);

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not create OpenCL program from source");

	status = clBuildProgram(program, 1, &deviceId, NULL, NULL, NULL);

	if (status == CL_BUILD_PROGRAM_FAILURE)
	{
		clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &textLength);
		char* buildLog = new char[textLength];
		clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, textLength, buildLog, NULL);
		log->logInfo("OpenCL build error:\n\n%s", buildLog);
		delete buildLog;
	}

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not build OpenCL program");

	kernel = clCreateKernel(program, "test_kernel", &status);

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not create OpenCL kernel");
}
