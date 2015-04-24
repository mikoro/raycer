// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/opencl.h>
#include <GL/glew.h>

#include "GpuRaytracing/GpuRaytracer.h"
#include "Raytracing/Raytracer.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Utils/Errors.h"
#include "Utils/OpenCL.h"
#include "Rendering/Framebuffer.h"
#include "Raytracing/Scene.h"
#include "Utils/Image.h"
#include "Runners/InteractiveRunner.h"

using namespace Raycer;

GpuRaytracer::GpuRaytracer()
{
}

GpuRaytracer::~GpuRaytracer()
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

void GpuRaytracer::initialize()
{
	OpenCL& openCL = App::getOpenCL();
	cl_int status = 0;

	infoPtr = clCreateBuffer(openCL.context, CL_MEM_READ_ONLY, sizeof(Gpu::Info), NULL, &status);
	checkCLError(status, "Could not create OpenCL info buffer");

	cameraPtr = clCreateBuffer(openCL.context, CL_MEM_READ_ONLY, sizeof(Gpu::Camera), NULL, &status);
	checkCLError(status, "Could not create OpenCL camera buffer");

	lightsPtr = clCreateBuffer(openCL.context, CL_MEM_READ_ONLY, sizeof(Gpu::Light) * MAX_LIGHTS, NULL, &status);
	checkCLError(status, "Could not create OpenCL lights buffer");

	planesPtr = clCreateBuffer(openCL.context, CL_MEM_READ_ONLY, sizeof(Gpu::Plane) * MAX_PLANES, NULL, &status);
	checkCLError(status, "Could not create OpenCL planes buffer");

	spheresPtr = clCreateBuffer(openCL.context, CL_MEM_READ_ONLY, sizeof(Gpu::Sphere) * MAX_SPHERES, NULL, &status);
	checkCLError(status, "Could not create OpenCL spheres buffer");
}

void GpuRaytracer::resizePixelBuffer(int width, int height)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	OpenCL& openCL = App::getOpenCL();

	bufferWidth = width;
	bufferHeight = height;

	releasePixelBuffer();
	cl_int status = 0;

	if (settings.general.interactive)
	{
		pixelsPtr = clCreateFromGLTexture2D(openCL.context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, framebuffer.getGpuTextureId(), &status);
		checkCLError(status, "Could not create OpenCL image from OpenGL texture");
	}
	else
	{
		cl_image_format imageFormat;
		imageFormat.image_channel_data_type = CL_FLOAT;
		imageFormat.image_channel_order = CL_RGBA;

		pixelsPtr = clCreateImage2D(openCL.context, CL_MEM_WRITE_ONLY, &imageFormat, bufferWidth, bufferHeight, 0, NULL, &status);
		checkCLError(status, "Could not create OpenCL image");
	}
}

void GpuRaytracer::releasePixelBuffer()
{
	if (pixelsPtr != nullptr)
	{
		checkCLError(clReleaseMemObject(pixelsPtr), "Could not release OpenCL memory object");
		pixelsPtr = nullptr;
	}
}

void GpuRaytracer::readScene(const Scene& scene)
{
	Settings& settings = App::getSettings();
	InteractiveRunner& interactiveRunner = App::getInteractiveRunner();

	gpuScene.readScene(scene);

	if (gpuScene.lights.size() > MAX_LIGHTS)
		throw std::runtime_error("Too many lights");

	if (gpuScene.planes.size() > MAX_PLANES)
		throw std::runtime_error("Too many planes");

	if (gpuScene.spheres.size() > MAX_SPHERES)
		throw std::runtime_error("Too many spheres");

	gpuScene.info.width = (float)bufferWidth;
	gpuScene.info.height = (float)bufferHeight;

	if (settings.general.interactive)
		gpuScene.info.time = (float)interactiveRunner.getElapsedTime();
	else
		gpuScene.info.time = 1.0f;

	gpuScene.info.lightCount = (cl_int)gpuScene.lights.size();
	gpuScene.info.planeCount = (cl_int)gpuScene.planes.size();
	gpuScene.info.sphereCount = (cl_int)gpuScene.spheres.size();
}

void GpuRaytracer::uploadData()
{
	OpenCL& openCL = App::getOpenCL();
	cl_int status = 0;

	status = clEnqueueWriteBuffer(openCL.commandQueue, infoPtr, CL_FALSE, 0, sizeof(Gpu::Info), &gpuScene.info, 0, NULL, NULL);
	checkCLError(status, "Could not write OpenCL info buffer");

	status = clEnqueueWriteBuffer(openCL.commandQueue, cameraPtr, CL_FALSE, 0, sizeof(Gpu::Camera), &gpuScene.camera, 0, NULL, NULL);
	checkCLError(status, "Could not write OpenCL camera buffer");

	status = clEnqueueWriteBuffer(openCL.commandQueue, lightsPtr, CL_FALSE, 0, sizeof(Gpu::Light) * gpuScene.lights.size(), &gpuScene.lights[0], 0, NULL, NULL);
	checkCLError(status, "Could not write OpenCL lights buffer");

	status = clEnqueueWriteBuffer(openCL.commandQueue, planesPtr, CL_FALSE, 0, sizeof(Gpu::Plane) * gpuScene.planes.size(), &gpuScene.planes[0], 0, NULL, NULL);
	checkCLError(status, "Could not write OpenCL planes buffer");

	status = clEnqueueWriteBuffer(openCL.commandQueue, spheresPtr, CL_FALSE, 0, sizeof(Gpu::Sphere) * gpuScene.spheres.size(), &gpuScene.spheres[0], 0, NULL, NULL);
	checkCLError(status, "Could not write OpenCL spheres buffer");
}

void GpuRaytracer::trace(std::atomic<bool>& interrupted)
{
	(void)interrupted;
	
	Settings& settings = App::getSettings();
	OpenCL& openCL = App::getOpenCL();

	if (settings.general.interactive)
	{
		glFinish();
		checkCLError(clEnqueueAcquireGLObjects(openCL.commandQueue, 1, &pixelsPtr, 0, NULL, NULL), "Could not enqueue OpenCL GL object acquire");
	}

	checkCLError(clSetKernelArg(openCL.raytraceKernel, 0, sizeof(cl_mem), &infoPtr), "Could not set OpenCL kernel argument (info)");
	checkCLError(clSetKernelArg(openCL.raytraceKernel, 1, sizeof(cl_mem), &cameraPtr), "Could not set OpenCL kernel argument (camera)");
	checkCLError(clSetKernelArg(openCL.raytraceKernel, 2, sizeof(cl_mem), &lightsPtr), "Could not set OpenCL kernel argument (lights)");
	checkCLError(clSetKernelArg(openCL.raytraceKernel, 3, sizeof(cl_mem), &planesPtr), "Could not set OpenCL kernel argument (planes)");
	checkCLError(clSetKernelArg(openCL.raytraceKernel, 4, sizeof(cl_mem), &spheresPtr), "Could not set OpenCL kernel argument (spheres)");
 	checkCLError(clSetKernelArg(openCL.raytraceKernel, 5, sizeof(cl_mem), &pixelsPtr), "Could not set OpenCL kernel argument (pixels)");

	const size_t globalSizes[] = { (size_t)bufferWidth, (size_t)bufferHeight };
	//const size_t localSizes[] = { 8, 8 }; // global_work_size needs to be evenly divisible by work-group size

	checkCLError(clEnqueueNDRangeKernel(openCL.commandQueue, openCL.raytraceKernel, 2, NULL, &globalSizes[0], NULL, 0, NULL, NULL), "Could not enqueue OpenCL kernel");

	if (settings.general.interactive)
		checkCLError(clEnqueueReleaseGLObjects(openCL.commandQueue, 1, &pixelsPtr, 0, NULL, NULL), "Could not enqueue OpenCL GL object release");

	checkCLError(clFinish(openCL.commandQueue), "Could not finish OpenCL command queue");
}

void GpuRaytracer::downloadImage()
{
	Log& log = App::getLog();
	OpenCL& openCL = App::getOpenCL();

	log.logInfo("Downloading image data from the OpenCL device");

	size_t origin[3] = { 0, 0, 0 };
	size_t region[3] = { bufferWidth, bufferHeight, 1 };

	std::vector<float> pixelData(bufferWidth * bufferHeight * 4);

	cl_int status = clEnqueueReadImage(openCL.commandQueue, pixelsPtr, CL_TRUE, &origin[0], &region[0], 0, 0, &pixelData[0], 0, NULL, NULL);
	checkCLError(status, "Could not read OpenCL image buffer");

	image.setSize(bufferWidth, bufferHeight);
	int length = bufferWidth * bufferHeight * 4;

	for (int i = 0; i < length; i += 4)
	{
		float r = pixelData[i];
		float g = pixelData[i + 1];
		float b = pixelData[i + 2];
		float a = pixelData[i + 3];

		image.setPixel(i / 4, Color(r, g, b, a).clamped());
	}
}

Image& GpuRaytracer::getImage()
{
	return image;
}

void GpuRaytracer::printSizes()
{
	OpenCL& openCL = App::getOpenCL();

	const size_t globalSize = 1;
	checkCLError(clEnqueueNDRangeKernel(openCL.commandQueue, openCL.printSizesKernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL), "Could not enqueue OpenCL kernel");
	checkCLError(clFinish(openCL.commandQueue), "Could not finish OpenCL command queue");
}
