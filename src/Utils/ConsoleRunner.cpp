// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#ifdef WIN32
#include <windows.h>
#endif

#include <thread>
#include <atomic>

#include "Utils/ConsoleRunner.h"
#include "Rendering/Image.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Raytracer.h"
#include "Raytracing/Material.h"
#include "Raytracing/Sphere.h"
#include "Raytracing/Plane.h"

using namespace Raycer;

#ifdef WIN32

namespace
{
	std::atomic<bool> interrupted = false;

	BOOL consoleCtrlHandler(DWORD fdwCtrlType)
	{
		if (fdwCtrlType == CTRL_C_EVENT)
		{
			interrupted = true;
			return true;
		}
		else
			return false;
	}
}

#endif

int ConsoleRunner::run(const ConsoleRunnerSettings& settings)
{
#ifdef WIN32
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)consoleCtrlHandler, TRUE);
#endif

	auto startTime = system_clock::now();

	Image image = Image(settings.width, settings.height);
	Scene scene = Scene(settings.sceneFileName);

	scene.initialize();
	scene.camera.setImagePlaneSize(settings.width, settings.height);
	scene.camera.calculateVariables();

	int totalPixelCount = settings.width * settings.height;

	printf("\nImage dimensions: %dx%d (%d pixels)\n", settings.width, settings.height, totalPixelCount);
	printf("Primitives: %d\n", (int)scene.primitives.size());
	printf("Lights: %d\n", (int)scene.lights.size());
	printf("Max reflections: %d\n\n", scene.maxReflections);
	printf("Tracing...\n\n");

	std::atomic<int> pixelCount = 0;
	std::atomic<int> rayCount = 0;
	std::atomic<bool> finished = false;

	auto renderFunction = [&]()
	{
		Raytracer::traceFast(image, scene, interrupted, pixelCount, rayCount);
		finished = true;
	};

	std::thread renderThread(renderFunction);

	while (!finished)
	{
		printProgress(startTime, totalPixelCount, pixelCount, rayCount);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	renderThread.join();

	printProgress(startTime, totalPixelCount, pixelCount, rayCount);

	if (!interrupted)
		printf("\n\nFinished!\n\n");
	else
		printf("\n\nInterrupted!\n\n");

	auto elapsedTime = system_clock::now() - startTime;
	int64_t elapsedMilliseconds = duration_cast<milliseconds>(elapsedTime).count();

	if (elapsedMilliseconds >= 1000)
		printf("Total time: %02d:%02d:%02d\n", (int)duration_cast<hours>(elapsedTime).count(), (int)duration_cast<minutes>(elapsedTime).count(), (int)duration_cast<seconds>(elapsedTime).count());
	else
		printf("Total time: %d ms\n", (int)elapsedMilliseconds);

	printf("Total pixels: %d\n", pixelCount.load());
	printf("Total rays: %d\n", rayCount.load());

	if (!interrupted)
	{
		printf("\nWriting the image file...\n");
		image.saveAs(settings.outputFileName);

		if (settings.viewImage)
		{
#ifdef WIN32
			ShellExecuteA(NULL, "open", settings.outputFileName.c_str(), NULL, NULL, SW_SHOWNORMAL);
#endif
		}
	}

	return 0;
}

void ConsoleRunner::printProgress(const time_point<system_clock>& startTime, int totalPixelCount, int pixelCount, int rayCount)
{
	auto elapsedTime = system_clock::now() - startTime;
	double elapsedSeconds = (double)duration_cast<milliseconds>(elapsedTime).count() / 1000.0;
	double msPerPixel = 0;

	if (pixelCount > 0)
		msPerPixel = (double)duration_cast<milliseconds>(elapsedTime).count() / pixelCount;

	auto estimatedTime = milliseconds((int)(msPerPixel * totalPixelCount + 0.5));
	auto remainingTime = estimatedTime - elapsedTime;

	int percentage = (int)(((double)pixelCount / totalPixelCount) * 100.0 + 0.5);
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
	printf("Pixels/s: %.2f | ", pixelCount / elapsedSeconds);
	printf("Rays/s: %.2f", rayCount / elapsedSeconds);

	printf("\r");
}
