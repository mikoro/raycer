// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/opencl.h>
#include <GL/glew.h>

#include "CLRaytracing/CLRaytracer.h"
#include "Raytracing/Raytracer.h"
#include "Raytracing/RaytracerState.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Utils/Errors.h"
#include "CLRaytracing/CLManager.h"
#include "Rendering/Framebuffer.h"
#include "Raytracing/Scene.h"
#include "Rendering/Image.h"
#include "Runners/InteractiveRunner.h"

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
	checkCLError(status, "Could not create OpenCL info buffer");

	cameraPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Camera), NULL, &status);
	checkCLError(status, "Could not create OpenCL camera buffer");

	lightsPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Light) * MAX_LIGHTS, NULL, &status);
	checkCLError(status, "Could not create OpenCL lights buffer");

	planesPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Plane) * MAX_PLANES, NULL, &status);
	checkCLError(status, "Could not create OpenCL planes buffer");

	spheresPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Sphere) * MAX_SPHERES, NULL, &status);
	checkCLError(status, "Could not create OpenCL spheres buffer");
}

void CLRaytracer::resizePixelBuffer(int width, int height)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	CLManager& clManager = App::getCLManager();

	bufferWidth = width;
	bufferHeight = height;

	releasePixelBuffer();
	cl_int status = 0;

	if (settings.general.interactive)
	{
		pixelsPtr = clCreateFromGLTexture2D(clManager.context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, framebuffer.getTextureId(), &status);
		checkCLError(status, "Could not create OpenCL image from OpenGL texture");
	}
	else
	{
		cl_image_format imageFormat;
		imageFormat.image_channel_data_type = CL_FLOAT;
		imageFormat.image_channel_order = CL_RGBA;

		pixelsPtr = clCreateImage2D(clManager.context, CL_MEM_WRITE_ONLY, &imageFormat, bufferWidth, bufferHeight, 0, NULL, &status);
		checkCLError(status, "Could not create OpenCL image");
	}
}

void CLRaytracer::releasePixelBuffer()
{
	if (pixelsPtr != nullptr)
	{
		checkCLError(clReleaseMemObject(pixelsPtr), "Could not release OpenCL memory object");
		pixelsPtr = nullptr;
	}
}

void CLRaytracer::convertSceneData(const Scene& scene)
{
	Settings& settings = App::getSettings();
	InteractiveRunner& interactiveRunner = App::getInteractiveRunner();

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
		clScene.state.time = (float)interactiveRunner.getElapsedTime();
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
	checkCLError(status, "Could not write OpenCL info buffer");

	status = clEnqueueWriteBuffer(clManager.commandQueue, cameraPtr, CL_FALSE, 0, sizeof(OpenCL::Camera), &clScene.camera, 0, NULL, NULL);
	checkCLError(status, "Could not write OpenCL camera buffer");

	status = clEnqueueWriteBuffer(clManager.commandQueue, lightsPtr, CL_FALSE, 0, sizeof(OpenCL::Light) * clScene.lights.size(), &clScene.lights[0], 0, NULL, NULL);
	checkCLError(status, "Could not write OpenCL lights buffer");

	status = clEnqueueWriteBuffer(clManager.commandQueue, planesPtr, CL_FALSE, 0, sizeof(OpenCL::Plane) * clScene.planes.size(), &clScene.planes[0], 0, NULL, NULL);
	checkCLError(status, "Could not write OpenCL planes buffer");

	status = clEnqueueWriteBuffer(clManager.commandQueue, spheresPtr, CL_FALSE, 0, sizeof(OpenCL::Sphere) * clScene.spheres.size(), &clScene.spheres[0], 0, NULL, NULL);
	checkCLError(status, "Could not write OpenCL spheres buffer");
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
		checkCLError(clEnqueueAcquireGLObjects(clManager.commandQueue, 1, &pixelsPtr, 0, NULL, NULL), "Could not enqueue OpenCL GL object acquire");
	}

	checkCLError(clSetKernelArg(clManager.raytraceKernel, 0, sizeof(cl_mem), &infoPtr), "Could not set OpenCL kernel argument (info)");
	checkCLError(clSetKernelArg(clManager.raytraceKernel, 1, sizeof(cl_mem), &cameraPtr), "Could not set OpenCL kernel argument (camera)");
	checkCLError(clSetKernelArg(clManager.raytraceKernel, 2, sizeof(cl_mem), &lightsPtr), "Could not set OpenCL kernel argument (lights)");
	checkCLError(clSetKernelArg(clManager.raytraceKernel, 3, sizeof(cl_mem), &planesPtr), "Could not set OpenCL kernel argument (planes)");
	checkCLError(clSetKernelArg(clManager.raytraceKernel, 4, sizeof(cl_mem), &spheresPtr), "Could not set OpenCL kernel argument (spheres)");
 	checkCLError(clSetKernelArg(clManager.raytraceKernel, 5, sizeof(cl_mem), &pixelsPtr), "Could not set OpenCL kernel argument (pixels)");

	const size_t globalSizes[] = { (size_t)bufferWidth, (size_t)bufferHeight };
	//const size_t localSizes[] = { 8, 8 }; // global_work_size needs to be evenly divisible by work-group size

	checkCLError(clEnqueueNDRangeKernel(clManager.commandQueue, clManager.raytraceKernel, 2, NULL, &globalSizes[0], NULL, 0, NULL, NULL), "Could not enqueue OpenCL kernel");

	if (settings.general.interactive)
		checkCLError(clEnqueueReleaseGLObjects(clManager.commandQueue, 1, &pixelsPtr, 0, NULL, NULL), "Could not enqueue OpenCL GL object release");

	checkCLError(clFinish(clManager.commandQueue), "Could not finish OpenCL command queue");
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
	checkCLError(status, "Could not read OpenCL image buffer");

	image.load(bufferWidth, bufferHeight, &data[0]);

	return image;
}

void CLRaytracer::printStructSizes()
{
	CLManager& clManager = App::getCLManager();

	const size_t globalSize = 1;
	checkCLError(clEnqueueNDRangeKernel(clManager.commandQueue, clManager.printStructSizesKernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL), "Could not enqueue OpenCL kernel");
	checkCLError(clFinish(clManager.commandQueue), "Could not finish OpenCL command queue");
}
