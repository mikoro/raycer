// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cstddef>
#include <stdexcept>
#include <vector>

#include <GL/gl3w.h>

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#include "OpenCL/CLRaytracer.h"
#include "OpenCL/CLManager.h"
#include "OpenCL/CLStructs.h"
#include "Raytracing/RaytracerState.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Rendering/Framebuffer.h"
#include "Rendering/Image.h"
#include "Runners/WindowRunner.h"

using namespace Raycer;

CLRaytracer::CLRaytracer()
{
}

CLRaytracer::~CLRaytracer()
{
	if (pixelsPtr != nullptr)
	{
		clReleaseMemObject(pixelsPtr);
		pixelsPtr = nullptr;
	}

	if (infoPtr != nullptr)
	{
		clReleaseMemObject(infoPtr);
		infoPtr = nullptr;
	}

	if (cameraPtr != nullptr)
	{
		clReleaseMemObject(cameraPtr);
		cameraPtr = nullptr;
	}

	if (lightsPtr != nullptr)
	{
		clReleaseMemObject(lightsPtr);
		lightsPtr = nullptr;
	}

	if (planesPtr != nullptr)
	{
		clReleaseMemObject(planesPtr);
		planesPtr = nullptr;
	}

	if (spheresPtr != nullptr)
	{
		clReleaseMemObject(spheresPtr);
		spheresPtr = nullptr;
	}
}

void CLRaytracer::initialize()
{
	CLManager& clManager = App::getCLManager();
	cl_int status = 0;

	infoPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::State), NULL, &status);
	CLManager::checkError(status, "Could not create info buffer");

	cameraPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Camera), NULL, &status);
	CLManager::checkError(status, "Could not create camera buffer");

	lightsPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Light) * MAX_LIGHTS, NULL, &status);
	CLManager::checkError(status, "Could not create lights buffer");

	planesPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Plane) * MAX_PLANES, NULL, &status);
	CLManager::checkError(status, "Could not create planes buffer");

	spheresPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Sphere) * MAX_SPHERES, NULL, &status);
	CLManager::checkError(status, "Could not create spheres buffer");
}

void CLRaytracer::resizePixelsBuffer(int width, int height)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	CLManager& clManager = App::getCLManager();

	bufferWidth = width;
	bufferHeight = height;

	releasePixelsBuffer();
	cl_int status = 0;

	if (settings.general.interactive)
	{
		pixelsPtr = clCreateFromGLTexture2D(clManager.context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, framebuffer.getImageTextureId(), &status);
		CLManager::checkError(status, "Could not create image from OpenGL texture");
	}
	else
	{
		cl_image_format imageFormat;
		imageFormat.image_channel_data_type = CL_FLOAT;
		imageFormat.image_channel_order = CL_RGBA;

		pixelsPtr = clCreateImage2D(clManager.context, CL_MEM_WRITE_ONLY, &imageFormat, bufferWidth, bufferHeight, 0, NULL, &status);
		CLManager::checkError(status, "Could not create image");
	}
}

void CLRaytracer::releasePixelsBuffer()
{
	if (pixelsPtr != nullptr)
	{
		CLManager::checkError(clReleaseMemObject(pixelsPtr), "Could not release memory object");
		pixelsPtr = nullptr;
	}
}

void CLRaytracer::convertSceneData(const Scene& scene)
{
	Settings& settings = App::getSettings();
	WindowRunner& windowRunner = App::getWindowRunner();

	clScene.convertSceneData(scene);

	if ((int)clScene.lights.size() > MAX_LIGHTS)
		throw std::runtime_error("Too many lights");

	if ((int)clScene.planes.size() > MAX_PLANES)
		throw std::runtime_error("Too many planes");

	if ((int)clScene.spheres.size() > MAX_SPHERES)
		throw std::runtime_error("Too many spheres");

	clScene.state.imageWidth = (float)bufferWidth;
	clScene.state.imageHeight = (float)bufferHeight;

	if (settings.general.interactive)
		clScene.state.time = (float)windowRunner.getElapsedTime();
	else
		clScene.state.time = 1.0f;

	clScene.state.lightCount = (cl_int)clScene.lights.size();
	clScene.state.planeCount = (cl_int)clScene.planes.size();
	clScene.state.sphereCount = (cl_int)clScene.spheres.size();
}

void CLRaytracer::uploadSceneData()
{
	CLManager& clManager = App::getCLManager();
	cl_int status = 0;

	status = clEnqueueWriteBuffer(clManager.commandQueue, infoPtr, CL_FALSE, 0, sizeof(OpenCL::State), &clScene.state, 0, NULL, NULL);
	CLManager::checkError(status, "Could not write info buffer");

	status = clEnqueueWriteBuffer(clManager.commandQueue, cameraPtr, CL_FALSE, 0, sizeof(OpenCL::Camera), &clScene.camera, 0, NULL, NULL);
	CLManager::checkError(status, "Could not write camera buffer");

	status = clEnqueueWriteBuffer(clManager.commandQueue, lightsPtr, CL_FALSE, 0, sizeof(OpenCL::Light) * clScene.lights.size(), &clScene.lights[0], 0, NULL, NULL);
	CLManager::checkError(status, "Could not write lights buffer");

	status = clEnqueueWriteBuffer(clManager.commandQueue, planesPtr, CL_FALSE, 0, sizeof(OpenCL::Plane) * clScene.planes.size(), &clScene.planes[0], 0, NULL, NULL);
	CLManager::checkError(status, "Could not write planes buffer");

	status = clEnqueueWriteBuffer(clManager.commandQueue, spheresPtr, CL_FALSE, 0, sizeof(OpenCL::Sphere) * clScene.spheres.size(), &clScene.spheres[0], 0, NULL, NULL);
	CLManager::checkError(status, "Could not write spheres buffer");
}

void CLRaytracer::run(RaytracerState& state, std::atomic<bool>& interrupted)
{
	(void)interrupted;
	
	Settings& settings = App::getSettings();
	CLManager& clManager = App::getCLManager();

	convertSceneData(*state.scene);
	uploadSceneData();

	if (settings.general.interactive)
	{
		glFinish();
		CLManager::checkError(clEnqueueAcquireGLObjects(clManager.commandQueue, 1, &pixelsPtr, 0, NULL, NULL), "Could not enqueue GL object acquire");
	}

	CLManager::checkError(clSetKernelArg(clManager.raytraceKernel, 0, sizeof(cl_mem), &infoPtr), "Could not set kernel argument (info)");
	CLManager::checkError(clSetKernelArg(clManager.raytraceKernel, 1, sizeof(cl_mem), &cameraPtr), "Could not set kernel argument (camera)");
	CLManager::checkError(clSetKernelArg(clManager.raytraceKernel, 2, sizeof(cl_mem), &lightsPtr), "Could not set kernel argument (lights)");
	CLManager::checkError(clSetKernelArg(clManager.raytraceKernel, 3, sizeof(cl_mem), &planesPtr), "Could not set kernel argument (planes)");
	CLManager::checkError(clSetKernelArg(clManager.raytraceKernel, 4, sizeof(cl_mem), &spheresPtr), "Could not set kernel argument (spheres)");
	CLManager::checkError(clSetKernelArg(clManager.raytraceKernel, 5, sizeof(cl_mem), &pixelsPtr), "Could not set kernel argument (pixels)");

	const size_t globalSizes[] = { (size_t)bufferWidth, (size_t)bufferHeight };
	//const size_t localSizes[] = { 8, 8 }; // global_work_size needs to be evenly divisible by work-group size

	CLManager::checkError(clEnqueueNDRangeKernel(clManager.commandQueue, clManager.raytraceKernel, 2, NULL, &globalSizes[0], NULL, 0, NULL, NULL), "Could not enqueue kernel");

	if (settings.general.interactive)
		CLManager::checkError(clEnqueueReleaseGLObjects(clManager.commandQueue, 1, &pixelsPtr, 0, NULL, NULL), "Could not enqueue GL object release");

	CLManager::checkError(clFinish(clManager.commandQueue), "Could not finish command queue");
}

Image& CLRaytracer::getImage()
{
	Log& log = App::getLog();
	CLManager& clManager = App::getCLManager();

	log.logInfo("Downloading image data from the OpenCL device");

	size_t origin[3] = { 0, 0, 0 };
	size_t region[3] = { (size_t)bufferWidth, (size_t)bufferHeight, 1 };

	std::vector<float> data(bufferWidth * bufferHeight * 4);

	cl_int status = clEnqueueReadImage(clManager.commandQueue, pixelsPtr, CL_TRUE, &origin[0], &region[0], 0, 0, &data[0], 0, NULL, NULL);
	CLManager::checkError(status, "Could not read image buffer");

	image.load(bufferWidth, bufferHeight, &data[0]);

	return image;
}

void CLRaytracer::printStructSizes()
{
	CLManager& clManager = App::getCLManager();

	const size_t globalSize = 1;
	CLManager::checkError(clEnqueueNDRangeKernel(clManager.commandQueue, clManager.printStructSizesKernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL), "Could not enqueue kernel");
	CLManager::checkError(clFinish(clManager.commandQueue), "Could not finish command queue");
}
