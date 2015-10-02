// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "OpenCL/CLManager.h"
#include "OpenCL/CLStructs.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"

using namespace Raycer;
using namespace boost::filesystem;

namespace
{
	void CL_CALLBACK openCLErrorCallback(const char* message, const void* privateInfo, size_t cb, void* userData)
	{
		(void)privateInfo;
		(void)cb;
		(void)userData;

		std::string tempMessage = tfm::format("OpenCL error: %s", message);
		std::replace(tempMessage.begin(), tempMessage.end(), '\n', ' ');
		App::getLog().logError(tempMessage);
	}
}

CLManager::~CLManager()
{
	if (printSizesKernel != nullptr)
	{
		clReleaseKernel(printSizesKernel);
		printSizesKernel = nullptr;
	}

	if (printSizesProgram != nullptr)
	{
		clReleaseProgram(printSizesProgram);
		printSizesProgram = nullptr;
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

#ifdef _WIN32
	_putenv_s("CUDA_CACHE_DISABLE", "1");
#else
	setenv("CUDA_CACHE_DISABLE", "1", 1);
#endif

	cl_uint platformCount = 0;
	checkError(clGetPlatformIDs(0, nullptr, &platformCount), "Could not get platform count");

	if (platformCount == 0)
		throw std::runtime_error("Could not find any OpenCL platforms");

	if (settings.openCL.platformId > int(platformCount) - 1)
		throw std::runtime_error("Invalid OpenCL platform id");

	std::vector<cl_platform_id> platformIds(platformCount);
	checkError(clGetPlatformIDs(platformCount, &platformIds[0], nullptr), "Could not get platforms");
	platformId = platformIds[settings.openCL.platformId];

	size_t length = 0;
	clGetPlatformInfo(platformId, CL_PLATFORM_NAME, 0, nullptr, &length);
	std::vector<char> platformName(length);
	clGetPlatformInfo(platformId, CL_PLATFORM_NAME, length, &platformName[0], nullptr);

	clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, 0, nullptr, &length);
	std::vector<char> platformVersion(length);
	clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, length, &platformVersion[0], nullptr);

	log.logInfo("OpenCL platform: %s", &platformName[0]);
	log.logInfo("OpenCL platform version: %s", &platformVersion[0]);

	cl_uint deviceCount = 0;
	checkError(clGetDeviceIDs(platformId, settings.openCL.deviceType, 0, nullptr, &deviceCount), "Could not get device count");

	if (deviceCount == 0)
		throw std::runtime_error("Could not find any devices");

	if (settings.openCL.deviceId > int(deviceCount) - 1)
		throw std::runtime_error("Invalid device id");

	std::vector<cl_device_id> deviceIds(deviceCount);
	checkError(clGetDeviceIDs(platformId, settings.openCL.deviceType, deviceCount, &deviceIds[0], nullptr), "Could not get devices");
	deviceId = deviceIds[settings.openCL.deviceId];

	clGetDeviceInfo(deviceId, CL_DEVICE_NAME, 0, nullptr, &length);
	std::vector<char> deviceName(length);
	clGetDeviceInfo(deviceId, CL_DEVICE_NAME, length, &deviceName[0], nullptr);

	clGetDeviceInfo(deviceId, CL_DEVICE_VERSION, 0, nullptr, &length);
	std::vector<char> deviceVersion(length);
	clGetDeviceInfo(deviceId, CL_DEVICE_VERSION, length, &deviceVersion[0], nullptr);

	log.logInfo("OpenCL device: %s", &deviceName[0]);
	log.logInfo("OpenCL device version: %s", &deviceVersion[0]);

	cl_int status = 0;

	if (settings.general.interactive)
	{
		clGetDeviceInfo(deviceId, CL_DEVICE_EXTENSIONS, 0, nullptr, &length);
		std::vector<char> extensions(length);
		clGetDeviceInfo(deviceId, CL_DEVICE_EXTENSIONS, length, &extensions[0], nullptr);
		std::string extensionsStr(&extensions[0]);

		if (extensionsStr.find("_gl_sharing") == std::string::npos)
			throw std::runtime_error("OpenCL-OpenGL interoperation is not supported (try non-interactive mode)");

		cl_context_properties properties[] =
		{
#ifdef _WIN32
				CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platformId),
				CL_WGL_HDC_KHR, reinterpret_cast<cl_context_properties>(wglGetCurrentDC()),
				CL_GL_CONTEXT_KHR, reinterpret_cast<cl_context_properties>(wglGetCurrentContext()),
				0
#elif __linux
			CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platformId),
			CL_GLX_DISPLAY_KHR, reinterpret_cast<cl_context_properties>(glXGetCurrentDisplay()),
			CL_GL_CONTEXT_KHR, reinterpret_cast<cl_context_properties>(glXGetCurrentContext()),
			0
#elif __APPLE__
			CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, reinterpret_cast<cl_context_properties>(CGLGetShareGroup(CGLGetCurrentContext())),
			0
#endif
		};

		context = clCreateContext(properties, 1, &deviceId, openCLErrorCallback, nullptr, &status);
		checkError(status, "Could not create OpenCL-OpenGL interoperation device context");
	}
	else
	{
		context = clCreateContext(nullptr, 1, &deviceId, openCLErrorCallback, nullptr, &status);
		checkError(status, "Could not create device context");
	}

	commandQueue = clCreateCommandQueue(context, deviceId, 0, &status);
	checkError(status, "Could not create command queue");

	// 	std::vector<std::string> sourceFiles = { "data/opencl/structs.cl", "data/opencl/printSizes.cl" };
	// 	printSizesProgram = createProgram(sourceFiles);
	// 	printSizesKernel = createKernel(printSizesProgram, "printSizes");
}

cl_program CLManager::createProgram(const std::vector<std::string>& sourceFilePaths)
{
	Log& log = App::getLog();
	Settings& settings = App::getSettings();

	log.logInfo("Building OpenCL program");

	std::stringstream sourceStringSs;

	for (const std::string& sourceFilePath : sourceFilePaths)
	{
		std::ifstream file(sourceFilePath);

		if (!file.good())
			throw std::runtime_error(tfm::format("Could not open OpenCL source file: %s", sourceFilePath));

		sourceStringSs << file.rdbuf();
		file.close();
	}

	std::string sourceString = sourceStringSs.str();
	const char* sourceStringPtr = sourceString.c_str();

	cl_int status = 0;
	size_t length = 0;

	cl_program program = clCreateProgramWithSource(context, 1, &sourceStringPtr, nullptr, &status);
	checkError(status, "Could not read program source file");

	std::string optionsString = tfm::format("%s -I%s/data/opencl", settings.openCL.options, boost::filesystem::current_path().string());
	status = clBuildProgram(program, 1, &deviceId, optionsString.c_str(), nullptr, nullptr);

	if (status == CL_BUILD_PROGRAM_FAILURE)
	{
		clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, 0, nullptr, &length);
		std::vector<char> buildLog(length);
		clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, length, &buildLog[0], nullptr);
		log.logInfo("OpenCL build error:\n\n%s", &buildLog[0]);
	}

	checkError(status, "Could not build program");

	return program;
}

cl_kernel CLManager::createKernel(cl_program program, const std::string& kernelName)
{
	cl_int status = 0;

	cl_kernel kernel = clCreateKernel(program, kernelName.c_str(), &status);
	checkError(status, tfm::format("Could not create kernel: %s", kernelName));

	return kernel;
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

void CLManager::writeStructSizes(const std::string& fileName)
{
	std::ofstream file(fileName);

	file << tfm::format("State: %d\n", sizeof(OpenCL::State));
	file << tfm::format("Camera: %d\n", sizeof(OpenCL::Camera));
	file << tfm::format("Raytracer: %d\n", sizeof(OpenCL::Raytracer));
	file << tfm::format("ToneMapper: %d\n", sizeof(OpenCL::ToneMapper));
	file << tfm::format("SimpleFog: %d\n", sizeof(OpenCL::SimpleFog));
	file << tfm::format("Material: %d\n", sizeof(OpenCL::Material));
	file << tfm::format("AmbientLight: %d\n", sizeof(OpenCL::AmbientLight));
	file << tfm::format("DirectionalLight: %d\n", sizeof(OpenCL::DirectionalLight));
	file << tfm::format("PointLight: %d\n", sizeof(OpenCL::PointLight));
	file << tfm::format("Triangle: %d\n", sizeof(OpenCL::Triangle));
	file << tfm::format("AABB: %d\n", sizeof(OpenCL::AABB));
	file << tfm::format("BVHNode: %d\n", sizeof(OpenCL::BVHNode));

	file.close();
}
