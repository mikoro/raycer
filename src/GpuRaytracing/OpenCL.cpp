// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>
#include <fstream>
#include <string>

#ifdef WIN32
#include <windows.h>
#endif

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/opencl.h>

#include "GpuRaytracing/OpenCL.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"

using namespace Raycer;

namespace
{
	void CL_CALLBACK openClErrorCallback(const char* message, const void* privateInfo, size_t cb, void* userData)
	{
		(void)privateInfo;
		(void)cb;
		(void)userData;

		App::getLog().logError("OpenCL error: %s", message);
	}
}

OpenCL::OpenCL()
{
}

OpenCL::~OpenCL()
{
	if (raytraceKernel != nullptr)
	{
		clReleaseKernel(raytraceKernel);
		raytraceKernel = nullptr;
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

void OpenCL::initialize()
{
	Log& log = App::getLog();
	Settings& settings = App::getSettings();

	log.logInfo("Initializing OpenCL");

	cl_uint platformCount = 0;
	cl_int status = clGetPlatformIDs(0, NULL, &platformCount);

	if (status != CL_SUCCESS || platformCount == 0)
		throw std::runtime_error("Could not find any OpenCL platforms");

	if (settings.general.openCLPlatform > (int)platformCount - 1)
		throw std::runtime_error("Invalid OpenCL platform id");

	cl_platform_id* platformIds = new cl_platform_id[platformCount];
	status = clGetPlatformIDs(platformCount, platformIds, NULL);
	platformId = platformIds[settings.general.openCLPlatform];
	delete[] platformIds;

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not get OpenCL platform info");

	size_t length = 0;
	clGetPlatformInfo(platformId, CL_PLATFORM_NAME, 0, NULL, &length);
	char* platformName = new char[length];
	clGetPlatformInfo(platformId, CL_PLATFORM_NAME, length, platformName, NULL);

	clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, 0, NULL, &length);
	char* platformVersion = new char[length];
	clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, length, platformVersion, NULL);

	log.logInfo("OpenCL platform: %s", platformName);
	log.logInfo("OpenCL version: %s", platformVersion);

	delete[] platformName;
	delete[] platformVersion;

	cl_uint deviceCount = 0;
	status = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, 0, NULL, &deviceCount);

	if (status != CL_SUCCESS || deviceCount == 0)
		throw std::runtime_error("Could not find any OpenCL GPU devices");

	if (settings.general.openCLDevice > (int)deviceCount - 1)
		throw std::runtime_error("Invalid OpenCL device id");

	cl_device_id* deviceIds = new cl_device_id[deviceCount];
	status = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, deviceCount, deviceIds, NULL);
	deviceId = deviceIds[settings.general.openCLDevice];
	delete[] deviceIds;

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not get OpenCL GPU device info");

	clGetDeviceInfo(deviceId, CL_DEVICE_NAME, 0, NULL, &length);
	char* deviceName = new char[length];
	clGetDeviceInfo(deviceId, CL_DEVICE_NAME, length, deviceName, NULL);

	log.logInfo("OpenCL device: %s", deviceName);

	delete[] deviceName;

	if (settings.general.interactive)
	{
		clGetDeviceInfo(deviceId, CL_DEVICE_EXTENSIONS, 0, NULL, &length);
		char* extensions = new char[length];
		clGetDeviceInfo(deviceId, CL_DEVICE_EXTENSIONS, length, extensions, NULL);
		std::string extensionsStr(extensions);
		delete[] extensions;

		if (extensionsStr.find("_gl_sharing") == std::string::npos)
			throw std::runtime_error("OpenCL-OpenGL interoperation is not supported");

		cl_context_properties properties[] =
		{
#ifdef WIN32
			CL_CONTEXT_PLATFORM, (cl_context_properties)platformId,
			CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
			CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
			0
#endif
		};

		context = clCreateContext(properties, 1, &deviceId, openClErrorCallback, NULL, &status);

		if (status != CL_SUCCESS)
			throw std::runtime_error("Could not create OpenCL interop device context");
	}
	else
	{
		context = clCreateContext(NULL, 1, &deviceId, openClErrorCallback, NULL, &status);

		if (status != CL_SUCCESS)
			throw std::runtime_error("Could not create OpenCL device context");
	}

	commandQueue = clCreateCommandQueue(context, deviceId, 0, &status);

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not create OpenCL command queue");
}

void OpenCL::loadKernels()
{
	Log& log = App::getLog();
	log.logInfo("Compiling OpenCL source files");

	std::ifstream file("data/kernels/raytrace.cl");
	std::stringstream ss;
	ss << file.rdbuf();
	std::string fileString = ss.str();
	const char* fileStringPtr = fileString.c_str();

	cl_int status = 0;
	size_t length = 0;

	program = clCreateProgramWithSource(context, 1, &fileStringPtr, NULL, &status);

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not create OpenCL program from source");

	status = clBuildProgram(program, 1, &deviceId, NULL, NULL, NULL);

	if (status == CL_BUILD_PROGRAM_FAILURE)
	{
		clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &length);
		char* buildLog = new char[length];
		clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, length, buildLog, NULL);
		log.logInfo("OpenCL build error:\n\n%s", buildLog);
		delete[] buildLog;
	}

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not build OpenCL program");

#if 0
	log->logInfo("Outputting OpenCL binary to raytrace.bin");

	size_t binarySize = 0;
	clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &binarySize, NULL);

	if (binarySize > 0)
	{
		unsigned char* binary = new unsigned char[binarySize];
		clGetProgramInfo(program, CL_PROGRAM_BINARIES, binarySize, &binary, NULL);
		std::ofstream binaryFile("raytrace.bin", std::ios::binary);
		binaryFile.write((const char*)binary, binarySize);
		binaryFile.close();
		delete[] binary;
}
#endif

	raytraceKernel = clCreateKernel(program, "raytrace", &status);

	if (status != CL_SUCCESS)
		throw std::runtime_error("Could not create OpenCL kernel");
}
