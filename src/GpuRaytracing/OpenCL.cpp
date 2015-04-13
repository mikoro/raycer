// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>
#include <fstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/opencl.h>
#include <GL/glew.h>

#include "tinyformat/tinyformat.h"

#include "GpuRaytracing/OpenCL.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Utils/Errors.h"
#include "Rendering/Framebuffer.h"

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

OpenCL::OpenCL()
{
}

OpenCL::~OpenCL()
{
	if (pixels != nullptr)
	{
		clReleaseMemObject(pixels);
		pixels = nullptr;
	}

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
	checkClError(clGetPlatformIDs(0, NULL, &platformCount), "Could not get OpenCL platform count");

	if (platformCount == 0)
		throw std::runtime_error("Could not find any OpenCL platforms");

	if (settings.openCL.platformId > (int)platformCount - 1)
		throw std::runtime_error("Invalid OpenCL platform id");

	cl_platform_id* platformIds = new cl_platform_id[platformCount];
	checkClError(clGetPlatformIDs(platformCount, platformIds, NULL), "Could not get OpenCL platforms");
	platformId = platformIds[settings.openCL.platformId];
	delete[] platformIds;

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
	checkClError(clGetDeviceIDs(platformId, settings.openCL.deviceType, 0, NULL, &deviceCount), "Could not get OpenCL device count");

	if (deviceCount == 0)
		throw std::runtime_error("Could not find any OpenCL devices");

	if (settings.openCL.deviceId > (int)deviceCount - 1)
		throw std::runtime_error("Invalid OpenCL device id");

	cl_device_id* deviceIds = new cl_device_id[deviceCount];
	checkClError(clGetDeviceIDs(platformId, settings.openCL.deviceType, deviceCount, deviceIds, NULL), "Could not get OpenCL devices");
	deviceId = deviceIds[settings.openCL.deviceId];
	delete[] deviceIds;

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
		checkClError(status, "Could not create OpenCL interop device context");
	}
	else
	{
		context = clCreateContext(NULL, 1, &deviceId, openCLErrorCallback, NULL, &status);
		checkClError(status, "Could not create OpenCL device context");
	}

	commandQueue = clCreateCommandQueue(context, deviceId, 0, &status);
	checkClError(status, "Could not create OpenCL command queue");
}

void OpenCL::loadKernels()
{
	Log& log = App::getLog();
	Settings& settings = App::getSettings();

	log.logInfo("Building OpenCL programs");

	std::ifstream file("data/kernels/raytrace.cl");
	std::stringstream ss;
	ss << file.rdbuf();
	std::string fileString = ss.str();
	const char* fileStringPtr = fileString.c_str();

	cl_int status = 0;
	size_t length = 0;

	program = clCreateProgramWithSource(context, 1, &fileStringPtr, NULL, &status);
	checkClError(status, "Could not read OpenCL program source file");

	status = clBuildProgram(program, 1, &deviceId, settings.openCL.options.c_str(), NULL, NULL);

	if (status == CL_BUILD_PROGRAM_FAILURE)
	{
		clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &length);
		std::vector<char> buildLog(length);
		clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, length, &buildLog[0], NULL);
		log.logInfo("OpenCL build error:\n\n%s", &buildLog[0]);
	}

	checkClError(status, "Could not build OpenCL program");

#if 1 && defined(_DEBUG)
	log.logInfo("Writing OpenCL binary to raytrace.bin");

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
	checkClError(status, "Could not create OpenCL kernel");
}

void OpenCL::releaseMemoryObjects()
{
	if (pixels != nullptr)
	{
		checkClError(clReleaseMemObject(pixels), "Could not release OpenCL memory object");
		pixels = nullptr;
	}
}

void OpenCL::resizeBuffers(size_t width, size_t height)
{
	Log& log = App::getLog();
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();

	log.logInfo("Resizing OpenCL buffers");
	releaseMemoryObjects();
	cl_int status = 0;

	if (settings.general.interactive)
	{
		pixels = clCreateFromGLTexture2D(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, framebuffer.getGpuTextureId(), &status);
		checkClError(status, "Could not create OpenCL image from OpenGL texture");
	}
	else
	{
		cl_image_format imageFormat;
		imageFormat.image_channel_data_type = CL_FLOAT;
		imageFormat.image_channel_order = CL_RGBA;

		pixels = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &imageFormat, width, height, 0, NULL, &status);
		checkClError(status, "Could not create OpenCL image");
	}
}

void OpenCL::readBufferImage()
{

}

Image& OpenCL::getBufferImage()
{
	return bufferImage;
}
