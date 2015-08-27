// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>
#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl3w.h>

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <OpenGL/OpenGL.h>
#else
#include <CL/opencl.h>
#endif

#ifdef __linux
#include <GL/glx.h>
#endif

#include "tinyformat/tinyformat.h"

#include "OpenCL/CLManager.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"

using namespace Raycer;

namespace
{
	void CL_CALLBACK openCLErrorCallback(const char* message, const void* privateInfo, size_t cb, void* userData)
	{
		(void)privateInfo;
		(void)cb;
		(void)userData;

		App::getLog().logError("OpenCL error: %s", message);
	}
}

CLManager::~CLManager()
{
	if (mainKernel != nullptr)
	{
		clReleaseKernel(mainKernel);
		mainKernel = nullptr;
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

void CLManager::initialize()
{
	Log& log = App::getLog();
	Settings& settings = App::getSettings();

	log.logInfo("Initializing OpenCL");

	cl_uint platformCount = 0;
	checkError(clGetPlatformIDs(0, NULL, &platformCount), "Could not get platform count");

	if (platformCount == 0)
		throw std::runtime_error("Could not find any OpenCL platforms");

	if (settings.openCL.platformId > (int)platformCount - 1)
		throw std::runtime_error("Invalid OpenCL platform id");

	std::vector<cl_platform_id> platformIds(platformCount);
	checkError(clGetPlatformIDs(platformCount, &platformIds[0], NULL), "Could not get platforms");
	platformId = platformIds[settings.openCL.platformId];

	size_t length = 0;
	clGetPlatformInfo(platformId, CL_PLATFORM_NAME, 0, NULL, &length);
	std::vector<char> platformName(length);
	clGetPlatformInfo(platformId, CL_PLATFORM_NAME, length, &platformName[0], NULL);

	clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, 0, NULL, &length);
	std::vector<char> platformVersion(length);
	clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, length, &platformVersion[0], NULL);

	log.logInfo("OpenCL platform: %s", &platformName[0]);
	log.logInfo("OpenCL version: %s", &platformVersion[0]);

	cl_uint deviceCount = 0;
	checkError(clGetDeviceIDs(platformId, settings.openCL.deviceType, 0, NULL, &deviceCount), "Could not get device count");

	if (deviceCount == 0)
		throw std::runtime_error("Could not find any devices");

	if (settings.openCL.deviceId > (int)deviceCount - 1)
		throw std::runtime_error("Invalid device id");

	std::vector<cl_device_id> deviceIds(deviceCount);
	checkError(clGetDeviceIDs(platformId, settings.openCL.deviceType, deviceCount, &deviceIds[0], NULL), "Could not get devices");
	deviceId = deviceIds[settings.openCL.deviceId];

	clGetDeviceInfo(deviceId, CL_DEVICE_NAME, 0, NULL, &length);
	std::vector<char> deviceName(length);
	clGetDeviceInfo(deviceId, CL_DEVICE_NAME, length, &deviceName[0], NULL);

	log.logInfo("OpenCL device: %s", &deviceName[0]);

	cl_int status = 0;

	if (settings.general.interactive)
	{
		clGetDeviceInfo(deviceId, CL_DEVICE_EXTENSIONS, 0, NULL, &length);
		std::vector<char> extensions(length);
		clGetDeviceInfo(deviceId, CL_DEVICE_EXTENSIONS, length, &extensions[0], NULL);
		std::string extensionsStr(&extensions[0]);

		if (extensionsStr.find("_gl_sharing") == std::string::npos)
			throw std::runtime_error("OpenCL-OpenGL interoperation is not supported");

		cl_context_properties properties[] =
		{
#ifdef _WIN32
			CL_CONTEXT_PLATFORM, (cl_context_properties)platformId,
			CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
			CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
			0
#elif __linux
			CL_CONTEXT_PLATFORM, (cl_context_properties)platformId,
			CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
			CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
			0
#elif __APPLE__
			CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,(cl_context_properties)CGLGetShareGroup(CGLGetCurrentContext()),
			0
#endif
		};

		context = clCreateContext(properties, 1, &deviceId, openCLErrorCallback, NULL, &status);
		checkError(status, "Could not create OpenCL-OpenGL interoperation device context");
	}
	else
	{
		context = clCreateContext(NULL, 1, &deviceId, openCLErrorCallback, NULL, &status);
		checkError(status, "Could not create device context");
	}

	commandQueue = clCreateCommandQueue(context, deviceId, 0, &status);
	checkError(status, "Could not create command queue");
}

void CLManager::loadKernels()
{
	Log& log = App::getLog();
	Settings& settings = App::getSettings();

	log.logInfo("Building OpenCL programs");

	std::vector<std::string> filePaths;
	filePaths.push_back("data/opencl/structs.cl");
	filePaths.push_back("data/opencl/main.cl");

	std::stringstream sourceStringSs;

	for (std::string& filePath : filePaths)
	{
		std::ifstream file(filePath);

		if (!file.good())
			throw std::runtime_error("Could not open OpenCL source file");

		sourceStringSs << file.rdbuf();
		file.close();
	}

	std::string sourceString = sourceStringSs.str();
	const char* sourceStringPtr = sourceString.c_str();
	
	cl_int status = 0;
	size_t length = 0;

	program = clCreateProgramWithSource(context, 1, &sourceStringPtr, NULL, &status);
	checkError(status, "Could not read program source file");

	status = clBuildProgram(program, 1, &deviceId, settings.openCL.options.c_str(), NULL, NULL);

	if (status == CL_BUILD_PROGRAM_FAILURE)
	{
		clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &length);
		std::vector<char> buildLog(length);
		clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, length, &buildLog[0], NULL);
		log.logInfo("OpenCL build error:\n\n%s", &buildLog[0]);
	}

	checkError(status, "Could not build program");

#if 1 && defined(_DEBUG)
	log.logInfo("Writing OpenCL binary to opencl.bin");

	size_t binaryDataSize = 0;
	clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &binaryDataSize, NULL);

	if (binaryDataSize > 0)
	{
		std::vector<unsigned char> binaryData(binaryDataSize);
		unsigned char* binaryPtr = &binaryData[0];
		clGetProgramInfo(program, CL_PROGRAM_BINARIES, binaryDataSize, &binaryPtr, NULL);
		std::ofstream binaryFile("opencl.bin", std::ios::binary);
		binaryFile.write((const char*)&binaryData[0], binaryDataSize);
		binaryFile.close();
	}
#endif

	mainKernel = clCreateKernel(program, "main", &status);
	checkError(status, "Could not create main kernel");
}

void CLManager::checkError(int result, const std::string& message)
{
	Settings& settings = App::getSettings();

	if (result != CL_SUCCESS && settings.general.checkCLErrors)
		throw std::runtime_error(tfm::format("OpenCL error: %s: %s", message, getErrorMessage(result)));
}

std::string CLManager::getErrorMessage(int result)
{
	switch (result)
	{
		case 0: return "CL_SUCCESS";
		case -1: return "CL_DEVICE_NOT_FOUND";
		case -2: return "CL_DEVICE_NOT_AVAILABLE";
		case -3: return "CL_COMPILER_NOT_AVAILABLE";
		case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
		case -5: return "CL_OUT_OF_RESOURCES";
		case -6: return "CL_OUT_OF_HOST_MEMORY";
		case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
		case -8: return "CL_MEM_COPY_OVERLAP";
		case -9: return "CL_IMAGE_FORMAT_MISMATCH";
		case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
		case -11: return "CL_BUILD_PROGRAM_FAILURE";
		case -12: return "CL_MAP_FAILURE";
		case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
		case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
		case -15: return "CL_COMPILE_PROGRAM_FAILURE";
		case -16: return "CL_LINKER_NOT_AVAILABLE";
		case -17: return "CL_LINK_PROGRAM_FAILURE";
		case -18: return "CL_DEVICE_PARTITION_FAILED";
		case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
		case -30: return "CL_INVALID_VALUE";
		case -31: return "CL_INVALID_DEVICE_TYPE";
		case -32: return "CL_INVALID_PLATFORM";
		case -33: return "CL_INVALID_DEVICE";
		case -34: return "CL_INVALID_CONTEXT";
		case -35: return "CL_INVALID_QUEUE_PROPERTIES";
		case -36: return "CL_INVALID_COMMAND_QUEUE";
		case -37: return "CL_INVALID_HOST_PTR";
		case -38: return "CL_INVALID_MEM_OBJECT";
		case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
		case -40: return "CL_INVALID_IMAGE_SIZE";
		case -41: return "CL_INVALID_SAMPLER";
		case -42: return "CL_INVALID_BINARY";
		case -43: return "CL_INVALID_BUILD_OPTIONS";
		case -44: return "CL_INVALID_PROGRAM";
		case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
		case -46: return "CL_INVALID_KERNEL_NAME";
		case -47: return "CL_INVALID_KERNEL_DEFINITION";
		case -48: return "CL_INVALID_KERNEL";
		case -49: return "CL_INVALID_ARG_INDEX";
		case -50: return "CL_INVALID_ARG_VALUE";
		case -51: return "CL_INVALID_ARG_SIZE";
		case -52: return "CL_INVALID_KERNEL_ARGS";
		case -53: return "CL_INVALID_WORK_DIMENSION";
		case -54: return "CL_INVALID_WORK_GROUP_SIZE";
		case -55: return "CL_INVALID_WORK_ITEM_SIZE";
		case -56: return "CL_INVALID_GLOBAL_OFFSET";
		case -57: return "CL_INVALID_EVENT_WAIT_LIST";
		case -58: return "CL_INVALID_EVENT";
		case -59: return "CL_INVALID_OPERATION";
		case -60: return "CL_INVALID_GL_OBJECT";
		case -61: return "CL_INVALID_BUFFER_SIZE";
		case -62: return "CL_INVALID_MIP_LEVEL";
		case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
		case -64: return "CL_INVALID_PROPERTY";
		case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
		case -66: return "CL_INVALID_COMPILER_OPTIONS";
		case -67: return "CL_INVALID_LINKER_OPTIONS";
		case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";
		case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
		case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
		case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
		case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
		case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
		case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
		default: return "Unknown error";
	}
}
