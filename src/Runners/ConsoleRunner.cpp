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
#include "Utils/OpenCL.h"
#include "Utils/Image.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Raytracer.h"
#include "GpuRaytracing/GpuRaytracer.h"
#include "GpuRaytracing/Structs.h"

using namespace Raycer;
using namespace std::chrono;

int ConsoleRunner::run()
{
	Log& log = App::getLog();
	Settings& settings = App::getSettings();

	interrupted = false;

	Scene scene;
	//scene.loadFromFile(settings.scene.fileName);
	scene = Scene::createTestScene();
	scene.initialize();
	scene.camera.setImagePlaneSize(settings.image.width, settings.image.height);
	scene.camera.precalculate();

	RaytracerConfig config;
	config.renderTarget = &resultImage;
	config.scene = &scene;
	config.sceneWidth = settings.image.width;
	config.sceneHeight = settings.image.height;
	config.pixelOffset = 0;
	config.pixelCount = config.sceneWidth * config.sceneHeight;
	config.isInteractive = false;

	resultImage.setSize(config.sceneWidth, config.sceneHeight);

	run(config);

	if (!interrupted)
	{
		resultImage.flip();
		resultImage.swapBytes();
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

void ConsoleRunner::run(RaytracerConfig& config)
{
	Settings& settings = App::getSettings();
	OpenCL& openCL = App::getOpenCL();
	Raytracer& raytracer = App::getRaytracer();
	GpuRaytracer& gpuRaytracer = App::getGpuRaytracer();

	interrupted = false;

	if (settings.openCL.enabled && !openCLInitialized)
	{
		openCL.initialize();
		openCL.loadKernels();

		openCLInitialized = true;
	}

	if (settings.openCL.enabled)
	{
		gpuRaytracer.initialize();
		gpuRaytracer.resizePixelBuffer(config.sceneWidth, config.sceneHeight);
		gpuRaytracer.readScene(*config.scene);
		gpuRaytracer.uploadData();
	}
	
	std::atomic<bool> finished;

	auto renderFunction = [&]()
	{
		if (!settings.openCL.enabled)
			raytracer.trace(config, interrupted);
		else
			gpuRaytracer.trace(interrupted);

		finished = true;
	};

	std::cout << tfm::format("\nStart raytracing (size: %dx%d, offset: %d, pixels: %d)\n\n", config.sceneWidth, config.sceneHeight, config.pixelOffset, config.pixelCount);

	if (settings.openCL.enabled)
		config.pixelsProcessed = config.pixelCount / 2;

	auto startTime = system_clock::now();
	std::thread renderThread(renderFunction);

	while (!finished)
	{
		printProgress(startTime, config.pixelCount, config.pixelsProcessed, config.raysProcessed);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	renderThread.join();

	if (settings.openCL.enabled)
		config.pixelsProcessed = config.pixelCount;

	printProgress(startTime, config.pixelCount, config.pixelsProcessed, config.raysProcessed);

	auto elapsedTime = system_clock::now() - startTime;
	int hours = (int)duration_cast<std::chrono::hours>(elapsedTime).count();
	int minutes = (int)duration_cast<std::chrono::minutes>(elapsedTime).count();
	int seconds = (int)duration_cast<std::chrono::seconds>(elapsedTime).count();
	int milliseconds = (int)duration_cast<std::chrono::milliseconds>(elapsedTime).count();
	milliseconds = milliseconds - seconds * 1000;
	std::string timeString = tfm::format("%02d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);

	std::cout << tfm::format("\n\nRaytracing %s (time: %s, pixels: %d, rays: %d)\n\n", interrupted ? "interrupted" : "finished", timeString, config.pixelsProcessed.load(), config.raysProcessed.load());

	if (settings.openCL.enabled)
	{
		gpuRaytracer.downloadImage();
		resultImage = gpuRaytracer.getImage();
	}
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

	int hours = (int)duration_cast<std::chrono::hours>(remainingTime).count();
	int minutes = (int)duration_cast<std::chrono::minutes>(remainingTime).count() % 60;
	int seconds = (int)duration_cast<std::chrono::seconds>(remainingTime).count() % 60;

	printf("] ");
	printf("%d %% | ", percentage);
	printf("Remaining time: %02d:%02d:%02d | ", hours, minutes, seconds);
	printf("Pixels/s: %.1f | ", (double)pixelsProcessed / elapsedSeconds);
	printf("Rays/s: %.1f", (double)raysProcessed / elapsedSeconds);
	printf("          \r");
}
