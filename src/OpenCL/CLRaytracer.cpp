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

namespace
{
	void releaseMemObject(cl_mem object)
	{
		if (object != nullptr)
		{
			CLManager::checkError(clReleaseMemObject(object), "Could not release memory object");
			object = nullptr;
		}
	}
}

CLRaytracer::~CLRaytracer()
{
	releaseMemObject(outputImagePtr);
	releaseMemObject(statePtr);
	releaseMemObject(cameraPtr);
	releaseMemObject(raytracerPtr);
	releaseMemObject(toneMapperPtr);
	releaseMemObject(simpleFogPtr);
	releaseMemObject(materialsPtr);
	releaseMemObject(ambientLightPtr);
	releaseMemObject(directionalLightsPtr);
	releaseMemObject(pointLightsPtr);
	releaseMemObject(trianglesPtr);
	releaseMemObject(bvhNodesPtr);

	if (raytraceKernel != nullptr)
	{
		clReleaseKernel(raytraceKernel);
		raytraceKernel = nullptr;
	}

	if (raytraceProgram != nullptr)
	{
		clReleaseProgram(raytraceProgram);
		raytraceProgram = nullptr;
	}
}

void CLRaytracer::initialize(const Scene& scene)
{
	CLManager& clManager = App::getCLManager();

	clScene.readSceneFull(scene);
	createBuffers();
	uploadFullData();

	std::vector<std::string> sourceFiles = {
		"data/opencl/structs.cl",
		"data/opencl/constructors.cl",
		"data/opencl/camera.cl",
		"data/opencl/intersections.cl",
		"data/opencl/raytrace.cl"
	};

	raytraceProgram = clManager.createProgram(sourceFiles);
	raytraceKernel = clManager.createKernel(raytraceProgram, "raytrace");
}

void CLRaytracer::resizeImageBuffer(int width, int height)
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	CLManager& clManager = App::getCLManager();

	imageBufferWidth = width;
	imageBufferHeight = height;

	cl_int status = 0;

	// use OpenGL texture as an image
	if (settings.general.interactive)
	{
		outputImagePtr = clCreateFromGLTexture2D(clManager.context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, framebuffer.getImageTextureId(), &status);
		CLManager::checkError(status, "Could not create output image from OpenGL texture");
	}
	else // create own image
	{
		cl_image_format imageFormat;
		imageFormat.image_channel_data_type = CL_FLOAT;
		imageFormat.image_channel_order = CL_RGBA;

		outputImagePtr = clCreateImage2D(clManager.context, CL_MEM_WRITE_ONLY, &imageFormat, imageBufferWidth, imageBufferHeight, 0, NULL, &status);
		CLManager::checkError(status, "Could not create output image");
	}
}

void CLRaytracer::releaseImageBuffer()
{
	releaseMemObject(outputImagePtr);
}

void CLRaytracer::run(RaytracerState& state, std::atomic<bool>& interrupted)
{
	(void)interrupted;

	Settings& settings = App::getSettings();
	CLManager& clManager = App::getCLManager();
	WindowRunner& windowRunner = App::getWindowRunner();

	clScene.readSceneCamera(*state.scene);

	if (settings.general.interactive)
		clScene.state.time = (float)windowRunner.getElapsedTime();
	else
		clScene.state.time = 1.0f;

	uploadCameraData();

	if (settings.general.interactive)
	{
		glFinish();
		CLManager::checkError(clEnqueueAcquireGLObjects(clManager.commandQueue, 1, &outputImagePtr, 0, NULL, NULL), "Could not enqueue OpenGL object acquire");
	}

	CLManager::checkError(clSetKernelArg(raytraceKernel, 0, sizeof(cl_mem), &outputImagePtr), "Could not set kernel argument (output image)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, 1, sizeof(cl_mem), &statePtr), "Could not set kernel argument (state)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, 2, sizeof(cl_mem), &cameraPtr), "Could not set kernel argument (camera)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, 3, sizeof(cl_mem), &raytracerPtr), "Could not set kernel argument (raytracer)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, 4, sizeof(cl_mem), &toneMapperPtr), "Could not set kernel argument (tone mapper)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, 5, sizeof(cl_mem), &simpleFogPtr), "Could not set kernel argument (simple fog)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, 6, sizeof(cl_mem), &materialsPtr), "Could not set kernel argument (materials)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, 7, sizeof(cl_mem), &ambientLightPtr), "Could not set kernel argument (ambient light)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, 8, sizeof(cl_mem), &directionalLightsPtr), "Could not set kernel argument (directional lights)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, 9, sizeof(cl_mem), &pointLightsPtr), "Could not set kernel argument (point lights)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, 10, sizeof(cl_mem), &trianglesPtr), "Could not set kernel argument (triangles)");
	CLManager::checkError(clSetKernelArg(raytraceKernel, 11, sizeof(cl_mem), &bvhNodesPtr), "Could not set kernel argument (bvh nodes)");

	const size_t globalSizes[] = { (size_t)imageBufferWidth, (size_t)imageBufferHeight };

	CLManager::checkError(clEnqueueNDRangeKernel(clManager.commandQueue, raytraceKernel, 2, NULL, &globalSizes[0], NULL, 0, NULL, NULL), "Could not enqueue raytrace kernel");

	if (settings.general.interactive)
		CLManager::checkError(clEnqueueReleaseGLObjects(clManager.commandQueue, 1, &outputImagePtr, 0, NULL, NULL), "Could not enqueue OpenGL object release");

	CLManager::checkError(clFinish(clManager.commandQueue), "Could not finish command queue");
}

Image CLRaytracer::downloadImage()
{
	Log& log = App::getLog();
	CLManager& clManager = App::getCLManager();

	log.logInfo("Downloading image data from the OpenCL device");

	size_t origin[3] = { 0, 0, 0 };
	size_t region[3] = { (size_t)imageBufferWidth, (size_t)imageBufferHeight, 1 };

	std::vector<float> data(imageBufferWidth * imageBufferHeight * 4);

	cl_int status = clEnqueueReadImage(clManager.commandQueue, outputImagePtr, CL_TRUE, &origin[0], &region[0], 0, 0, &data[0], 0, NULL, NULL);
	CLManager::checkError(status, "Could not read output image buffer");

	return Image(imageBufferWidth, imageBufferHeight, &data[0]);
}

void CLRaytracer::createBuffers()
{
	CLManager& clManager = App::getCLManager();
	cl_int status = 0;

	releaseMemObject(statePtr);
	statePtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::State), NULL, &status);
	CLManager::checkError(status, "Could not create state buffer");

	releaseMemObject(cameraPtr);
	cameraPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Camera), NULL, &status);
	CLManager::checkError(status, "Could not create camera buffer");

	releaseMemObject(raytracerPtr);
	raytracerPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Raytracer), NULL, &status);
	CLManager::checkError(status, "Could not create raytracer buffer");

	releaseMemObject(toneMapperPtr);
	toneMapperPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::ToneMapper), NULL, &status);
	CLManager::checkError(status, "Could not create tone mapper buffer");

	releaseMemObject(simpleFogPtr);
	simpleFogPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::SimpleFog), NULL, &status);
	CLManager::checkError(status, "Could not create simple fog buffer");

	if (clScene.materials.size() > 0)
	{
		releaseMemObject(materialsPtr);
		materialsPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Material) * clScene.materials.size(), NULL, &status);
		CLManager::checkError(status, "Could not create materials buffer");
	}

	releaseMemObject(ambientLightPtr);
	ambientLightPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::AmbientLight), NULL, &status);
	CLManager::checkError(status, "Could not create ambient light buffer");

	if (clScene.directionalLights.size() > 0)
	{
		releaseMemObject(directionalLightsPtr);
		directionalLightsPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::DirectionalLight) * clScene.directionalLights.size(), NULL, &status);
		CLManager::checkError(status, "Could not create directional lights buffer");
	}

	if (clScene.pointLights.size() > 0)
	{
		releaseMemObject(pointLightsPtr);
		pointLightsPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::PointLight) * clScene.pointLights.size(), NULL, &status);
		CLManager::checkError(status, "Could not create point lights buffer");
	}

	if (clScene.triangles.size() > 0)
	{
		releaseMemObject(trianglesPtr);
		trianglesPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::Triangle) * clScene.triangles.size(), NULL, &status);
		CLManager::checkError(status, "Could not create triangles buffer");
	}

	if (clScene.bvhNodes.size() > 0)
	{
		releaseMemObject(bvhNodesPtr);
		bvhNodesPtr = clCreateBuffer(clManager.context, CL_MEM_READ_ONLY, sizeof(OpenCL::BVHNode) * clScene.bvhNodes.size(), NULL, &status);
		CLManager::checkError(status, "Could not create bvh nodes buffer");
	}
}

void CLRaytracer::uploadFullData()
{
	CLManager& clManager = App::getCLManager();
	cl_int status = 0;

	status = clEnqueueWriteBuffer(clManager.commandQueue, statePtr, CL_FALSE, 0, sizeof(OpenCL::State), &clScene.state, 0, NULL, NULL);
	CLManager::checkError(status, "Could not write state buffer");

	uploadCameraData();

	status = clEnqueueWriteBuffer(clManager.commandQueue, raytracerPtr, CL_FALSE, 0, sizeof(OpenCL::Raytracer), &clScene.raytracer, 0, NULL, NULL);
	CLManager::checkError(status, "Could not write raytracer buffer");

	status = clEnqueueWriteBuffer(clManager.commandQueue, toneMapperPtr, CL_FALSE, 0, sizeof(OpenCL::ToneMapper), &clScene.toneMapper, 0, NULL, NULL);
	CLManager::checkError(status, "Could not write tone mapper buffer");

	status = clEnqueueWriteBuffer(clManager.commandQueue, simpleFogPtr, CL_FALSE, 0, sizeof(OpenCL::SimpleFog), &clScene.simpleFog, 0, NULL, NULL);
	CLManager::checkError(status, "Could not write simple fog buffer");

	if (clScene.materials.size() > 0)
	{
		status = clEnqueueWriteBuffer(clManager.commandQueue, materialsPtr, CL_FALSE, 0, sizeof(OpenCL::Material) * clScene.materials.size(), &clScene.materials[0], 0, NULL, NULL);
		CLManager::checkError(status, "Could not write materials buffer");
	}

	status = clEnqueueWriteBuffer(clManager.commandQueue, ambientLightPtr, CL_FALSE, 0, sizeof(OpenCL::AmbientLight), &clScene.ambientLight, 0, NULL, NULL);
	CLManager::checkError(status, "Could not write ambient light buffer");

	if (clScene.directionalLights.size() > 0)
	{
		status = clEnqueueWriteBuffer(clManager.commandQueue, directionalLightsPtr, CL_FALSE, 0, sizeof(OpenCL::DirectionalLight) * clScene.directionalLights.size(), &clScene.directionalLights[0], 0, NULL, NULL);
		CLManager::checkError(status, "Could not write directional lights buffer");
	}

	if (clScene.pointLights.size() > 0)
	{
		status = clEnqueueWriteBuffer(clManager.commandQueue, pointLightsPtr, CL_FALSE, 0, sizeof(OpenCL::PointLight) * clScene.pointLights.size(), &clScene.pointLights[0], 0, NULL, NULL);
		CLManager::checkError(status, "Could not write point lights buffer");
	}

	if (clScene.triangles.size() > 0)
	{
		status = clEnqueueWriteBuffer(clManager.commandQueue, trianglesPtr, CL_FALSE, 0, sizeof(OpenCL::Triangle) * clScene.triangles.size(), &clScene.triangles[0], 0, NULL, NULL);
		CLManager::checkError(status, "Could not write triangles buffer");
	}

	if (clScene.bvhNodes.size() > 0)
	{
		status = clEnqueueWriteBuffer(clManager.commandQueue, bvhNodesPtr, CL_FALSE, 0, sizeof(OpenCL::BVHNode) * clScene.bvhNodes.size(), &clScene.bvhNodes[0], 0, NULL, NULL);
		CLManager::checkError(status, "Could not write bvh nodes buffer");
	}
}

void CLRaytracer::uploadCameraData()
{
	CLManager& clManager = App::getCLManager();
	cl_int status = 0;

	status = clEnqueueWriteBuffer(clManager.commandQueue, cameraPtr, CL_FALSE, 0, sizeof(OpenCL::Camera), &clScene.camera, 0, NULL, NULL);
	CLManager::checkError(status, "Could not write camera buffer");
}
