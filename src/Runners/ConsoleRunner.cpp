// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <thread>
#include <atomic>

#include "tinyformat/tinyformat.h"

#ifdef WIN32
#include <windows.h>
#endif

#include "Runners/ConsoleRunner.h"
#include "Utils/Log.h"
#include "Rendering/Image.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Raytracer.h"

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

ConsoleRunner::ConsoleRunner(BaseLog& baseLog_) : baseLog(baseLog_)
{
	log = baseLog_.getNamedLog("ConsoleRunner");
}

int ConsoleRunner::run(ConsoleSettings& settings)
{
#ifdef WIN32
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)consoleCtrlHandler, TRUE);
#endif

	log->logInfo("Initializing console runner");

	auto startTime = system_clock::now();

	Image image = Image(settings.width, settings.height);
	Scene scene = Scene(baseLog);

	scene.load(settings.sceneFileName);
	scene.initialize();
	scene.camera.setImagePlaneSize(settings.width, settings.height);
	scene.camera.calculateVariables();

	int totalPixelCount = settings.width * settings.height;
	log->logInfo("Start raytracing (size: %dx%d, pixels: %d, reflections: %d)", settings.width, settings.height, totalPixelCount, scene.maxReflections);

	std::atomic<int> pixelCount = 0;
	std::atomic<int> rayCount = 0;
	std::atomic<bool> finished = false;

	auto renderFunction = [&]()
	{
		Raytracer::traceFast(image, scene, interrupted, pixelCount, rayCount);
		finished = true;
	};

	printf("\n");
	std::thread renderThread(renderFunction);

	while (!finished)
	{
		printProgress(startTime, totalPixelCount, pixelCount, rayCount);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	renderThread.join();
	printProgress(startTime, totalPixelCount, pixelCount, rayCount);
	printf("\n\n");

	auto elapsedTime = system_clock::now() - startTime;
	std::string timeString = tfm::format("%02d:%02d:%02d.%03d", (int)duration_cast<hours>(elapsedTime).count(), (int)duration_cast<minutes>(elapsedTime).count(), (int)duration_cast<seconds>(elapsedTime).count(), (int)duration_cast<milliseconds>(elapsedTime).count());;

	log->logInfo("Raytracing %s (time: %s, rays: %d)", interrupted ? "interrupted" : "finished", timeString, rayCount.load());

	if (!interrupted)
	{
		log->logInfo("Saving the image to %s", settings.outputFileName);
		image.saveAs(settings.outputFileName);

		if (settings.viewImage)
		{
			log->logInfo("Opening the image in an external viewer");

#ifdef WIN32
			ShellExecuteA(NULL, "open", settings.outputFileName.c_str(), NULL, NULL, SW_SHOWNORMAL);
#endif
		}
	}

	log->logInfo("Closing console runner");

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
