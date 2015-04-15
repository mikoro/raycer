// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <thread>
#include <atomic>

#ifdef _WIN32
#include <windows.h>
#endif

#include "tinyformat/tinyformat.h"

#include "Runners/ConsoleRunner.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "GpuRaytracing/OpenCL.h"
#include "Rendering/Image.h"
#include "CpuRaytracing/Scene.h"
#include "CpuRaytracing/CpuRaytracer.h"
#include "CpuRaytracing/RaytraceInfo.h"
#include "GpuRaytracing/GpuRaytracer.h"

using namespace Raycer;
using namespace std::chrono;

int ConsoleRunner::run()
{
	Log& log = App::getLog();
	Settings& settings = App::getSettings();

	interrupted = false;

	Scene scene;
	scene.loadFromFile(settings.scene.fileName);
	scene.initialize();
	scene.camera.setImagePlaneSize(settings.image.width, settings.image.height);
	scene.camera.calculateVariables();

	RaytraceInfo info;
	info.renderTarget = &resultImage;
	info.scene = &scene;
	info.sceneWidth = settings.image.width;
	info.sceneHeight = settings.image.height;
	info.pixelOffset = 0;
	info.pixelCount = info.sceneWidth * info.sceneHeight;

	resultImage.setSize(info.sceneWidth, info.sceneHeight);

	run(info);

	if (!interrupted)
	{
		resultImage.saveAs(settings.image.fileName);

		if (settings.image.autoView)
		{
			log.logInfo("Opening the image in an external viewer");
#ifdef _WIN32
			ShellExecuteA(NULL, "open", settings.image.fileName.c_str(), NULL, NULL, SW_SHOWNORMAL);
#endif
		}
	}

	return 0;
}

void ConsoleRunner::run(RaytraceInfo& info)
{
	Settings& settings = App::getSettings();
	OpenCL& openCL = App::getOpenCL();
	CpuRaytracer& cpuRaytracer = App::getCpuRaytracer();
	GpuRaytracer& gpuRaytracer = App::getGpuRaytracer();

	interrupted = false;

	if (settings.openCL.enabled && !openCLInitialized)
	{
		openCL.initialize();
		openCL.loadKernels();

		openCLInitialized = true;
	}

	if (settings.openCL.enabled)
		openCL.resizeBuffers(info.sceneWidth, info.sceneHeight);

	std::atomic<bool> finished;

	auto renderFunction = [&]()
	{
		if (!settings.openCL.enabled)
			cpuRaytracer.trace(info, interrupted);
		else
			gpuRaytracer.trace(info, interrupted);

		finished = true;
	};

	std::cout << tfm::format("\nStart raytracing (size: %dx%d, offset: %d, pixels: %d)\n\n", info.sceneWidth, info.sceneHeight, info.pixelOffset, info.pixelCount);

	auto startTime = system_clock::now();
	std::thread renderThread(renderFunction);

	while (!finished)
	{
		printProgress(startTime, info.pixelCount, info.pixelsProcessed, info.raysProcessed);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	renderThread.join();
	printProgress(startTime, info.pixelCount, info.pixelsProcessed, info.raysProcessed);

	auto elapsedTime = system_clock::now() - startTime;
	int hours = (int)duration_cast<std::chrono::hours>(elapsedTime).count();
	int minutes = (int)duration_cast<std::chrono::minutes>(elapsedTime).count();
	int seconds = (int)duration_cast<std::chrono::seconds>(elapsedTime).count();
	int milliseconds = (int)duration_cast<std::chrono::milliseconds>(elapsedTime).count();
	milliseconds = milliseconds - seconds * 1000;
	std::string timeString = tfm::format("%02d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);

	std::cout << tfm::format("\n\nRaytracing %s (time: %s, rays: %d)\n\n", interrupted ? "interrupted" : "finished", timeString, info.raysProcessed.load());

	//if (settings.openCL.enabled)
		//resultImage = openCL.getBufferAsImage();
}

void ConsoleRunner::interrupt()
{
	interrupted = true;
}

Image& ConsoleRunner::getResultImage()
{
	return resultImage;
}

void ConsoleRunner::printProgress(const time_point<system_clock>& startTime, int totalPixelCount, int pixelsProcessed, int raysProcessed)
{
	auto elapsedTime = system_clock::now() - startTime;
	double elapsedSeconds = (double)duration_cast<milliseconds>(elapsedTime).count() / 1000.0;
	double msPerPixel = 0;

	if (pixelsProcessed > 0)
		msPerPixel = (double)duration_cast<milliseconds>(elapsedTime).count() / (double)pixelsProcessed;

	auto estimatedTime = milliseconds((int)(msPerPixel * (double)totalPixelCount + 0.5));
	auto remainingTime = estimatedTime - elapsedTime;

	int percentage = (int)(((double)pixelsProcessed / (double)totalPixelCount) * 100.0 + 0.5);
	int barCount = percentage / 4;

	printf("[");

	for (int i = 0; i < barCount; ++i)
		printf("=");

	if (barCount < 25)
	{
		printf(">");

		for (int i = 0; i < (24 - barCount); ++i)
			printf(" ");
	}

	printf("] ");
	printf("%d %% | ", percentage);
	printf("Remaining time: %02d:%02d:%02d | ", (int)duration_cast<hours>(remainingTime).count(), (int)duration_cast<minutes>(remainingTime).count(), (int)duration_cast<seconds>(remainingTime).count());
	printf("Pixels/s: %.2f | ", (double)pixelsProcessed / elapsedSeconds);
	printf("Rays/s: %.2f", (double)raysProcessed / elapsedSeconds);
	printf("\r");
}
