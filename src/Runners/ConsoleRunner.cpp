// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Runners/ConsoleRunner.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Utils/StringUtils.h"
#include "Rendering/Image.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Tracers/Tracer.h"
#include "Raytracing/Tracers/TracerState.h"
#include "Raytracing/Camera.h"
#include "OpenCL/CLManager.h"
#include "OpenCL/CLTracer.h"

using namespace Raycer;
using namespace std::chrono;

int ConsoleRunner::run()
{
	Settings& settings = App::getSettings();

	interrupted = false;

	pixelsPerSecondAverage.setAlpha(0.1);
	pixelsPerSecondAverage.setAverage(1.0);
	remainingTimeAverage.setAlpha(0.1);
	remainingTimeAverage.setAverage(0.0);

	Scene scene;

	if (settings.scene.enableTestScenes)
		scene = Scene::createTestScene(settings.scene.testSceneNumber);
	else
		scene = Scene::loadFromFile(settings.scene.fileName);

	scene.initialize();
	scene.camera.setImagePlaneSize(settings.image.width, settings.image.height);
	scene.camera.update(scene, 0.0);

	Image image(settings.image.width, settings.image.height);

	TracerState state;
	state.scene = &scene;
	state.linearImage = &image;
	state.toneMappedImage = &image;
	state.imageWidth = settings.image.width;
	state.imageHeight = settings.image.height;
	state.pixelStartOffset = 0;
	state.pixelCount = state.imageWidth * state.imageHeight;

	run(state);

	if (!interrupted)
	{
		image.save(settings.image.fileName);

		if (settings.image.autoView)
			openImageExternally(settings.image.fileName);
	}
	else
		image.save("partial_result.png");

	return 0;
}

void ConsoleRunner::run(TracerState& state)
{
	Settings& settings = App::getSettings();
	CLManager& clManager = App::getCLManager();
	CLTracer& clTracer = App::getCLTracer();

	interrupted = false;

	if (settings.openCL.enabled && !openCLInitialized)
	{
		clManager.initialize();
		openCLInitialized = true;
	}

	if (settings.openCL.enabled)
	{
		clTracer.initializeKernels();
		clTracer.initializeImageBuffer(state.imageWidth, state.imageHeight);
		clTracer.initializeBuffers(*state.scene);
	}

	std::atomic<bool> renderThreadFinished(false);
	std::exception_ptr renderThreadException = nullptr;

	auto renderThreadFunction = [&]()
	{
		try
		{
			if (!settings.openCL.enabled)
				Tracer::getTracer(state.scene->general.tracerType)->run(state, interrupted);
			else
				clTracer.run(state, interrupted);
		}
		catch (...)
		{
			renderThreadException = std::current_exception();
		}

		renderThreadFinished = true;
	};

	std::cout << tfm::format("\nStart tracing (dimensions: %dx%d, offset: %d, pixels: %d, size: %fB)\n\n",
		state.imageWidth,
		state.imageHeight,
		state.pixelStartOffset,
		state.pixelCount,
		StringUtils::humanizeNumber(double(state.pixelCount * sizeof(Color)), true));

	auto startTime = high_resolution_clock::now();
	std::thread renderThread(renderThreadFunction);

	while (!renderThreadFinished)
	{
		if (!settings.openCL.enabled)
			printProgress(startTime, state.pixelCount, state.pixelsProcessed);
		else
			printProgressOpenCL(startTime);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	renderThread.join();

	if (renderThreadException != nullptr)
		std::rethrow_exception(renderThreadException);

	if (!settings.openCL.enabled)
		printProgress(startTime, state.pixelCount, state.pixelsProcessed);
	else
		printProgressOpenCL(startTime);

	auto elapsedTime = high_resolution_clock::now() - startTime;
	auto totalElapsedSeconds = duration_cast<std::chrono::seconds>(elapsedTime).count();
	auto totalElapsedMilliseconds = duration_cast<std::chrono::milliseconds>(elapsedTime).count();
	auto elapsedHours = totalElapsedSeconds / 3600;
	auto elapsedMinutes = (totalElapsedSeconds - elapsedHours * 3600) / 60;
	auto elapsedSeconds = totalElapsedSeconds - elapsedHours * 3600 - elapsedMinutes * 60;
	auto elapsedMilliseconds = totalElapsedMilliseconds - totalElapsedSeconds * 1000;

	double totalPixelsPerSecond = 0.0;

	if (totalElapsedMilliseconds > 0)
		totalPixelsPerSecond = state.pixelsProcessed / (totalElapsedMilliseconds / 1000.0);

	std::string timeString = tfm::format("%02d:%02d:%02d.%03d", elapsedHours, elapsedMinutes, elapsedSeconds, elapsedMilliseconds);
	std::cout << tfm::format("\n\nTracing %s (time: %s, pixels/s: %f)\n\n",
		interrupted ? "interrupted" : "finished",
		timeString,
		StringUtils::humanizeNumber(totalPixelsPerSecond));

	if (!interrupted && settings.openCL.enabled)
		*state.toneMappedImage = clTracer.downloadImage();
}

void ConsoleRunner::interrupt()
{
	interrupted = true;
}

void ConsoleRunner::openImageExternally(const std::string& fileName)
{
	Log& log = App::getLog();
	log.logInfo("Opening the image in an external viewer");

#ifdef _WIN32
	ShellExecuteA(nullptr, "open", fileName.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
	int pid = fork();

	if (pid == 0)
	{
#ifdef __linux
		char* arg[] = { (char*)"xdg-open", (char*)fileName.c_str(), (char*)0 };
#elif __APPLE__
		char* arg[] = { (char*)"open", (char*)fileName.c_str(), (char*)0 };
#endif
		if (execvp(arg[0], arg) == -1)
			log.logWarning("Could not open the image (%d)", errno);
	}
#endif
}

void ConsoleRunner::printProgress(const time_point<high_resolution_clock>& startTime, size_t totalPixelCount, size_t pixelsProcessed)
{
	auto elapsedTime = high_resolution_clock::now() - startTime;
	double elapsedSeconds = duration_cast<std::chrono::milliseconds>(elapsedTime).count() / 1000.0;
	double msPerPixel = 0.0;

	if (pixelsProcessed > 0)
		msPerPixel = duration_cast<std::chrono::milliseconds>(elapsedTime).count() / double(pixelsProcessed);

	auto estimatedTime = std::chrono::milliseconds(int(msPerPixel * double(totalPixelCount) + 0.5));
	auto remainingTime = estimatedTime - elapsedTime;

	uint percentage = int((double(pixelsProcessed) / double(totalPixelCount)) * 100.0 + 0.5);
	uint barCount = percentage / 4;

	printf("[");

	for (uint i = 0; i < barCount; ++i)
		printf("=");

	if (barCount < 25)
	{
		printf(">");

		for (uint i = 0; i < (24 - barCount); ++i)
			printf(" ");
	}

	if (elapsedSeconds > 0.0)
		pixelsPerSecondAverage.addMeasurement(double(pixelsProcessed) / elapsedSeconds);

	remainingTimeAverage.addMeasurement(double(duration_cast<std::chrono::seconds>(remainingTime).count()));

	if (pixelsProcessed == totalPixelCount)
		remainingTimeAverage.setAverage(0.0);

	int totalRemainingSeconds = int(remainingTimeAverage.getAverage() + 0.5);
	int remainingHours = totalRemainingSeconds / 3600;
	int remainingMinutes = (totalRemainingSeconds - remainingHours * 3600) / 60;
	int remainingSeconds = totalRemainingSeconds - remainingHours * 3600 - remainingMinutes * 60;

	printf("] ");
	printf("%u %% | ", percentage);
	printf("Remaining time: %02d:%02d:%02d | ", remainingHours, remainingMinutes, remainingSeconds);
	printf("Pixels/s: %s", StringUtils::humanizeNumber(pixelsPerSecondAverage.getAverage()).c_str());
	printf("          \r");
}

void ConsoleRunner::printProgressOpenCL(const std::chrono::time_point<std::chrono::high_resolution_clock>& startTime)
{
	auto elapsedTime = high_resolution_clock::now() - startTime;
	int totalElapsedSeconds = int(duration_cast<std::chrono::seconds>(elapsedTime).count());
	int elapsedHours = totalElapsedSeconds / 3600;
	int elapsedMinutes = (totalElapsedSeconds - elapsedHours * 3600) / 60;
	int elapsedSeconds = totalElapsedSeconds - elapsedHours * 3600 - elapsedMinutes * 60;

	if (++openCLProgressCounter1 % 5 == 0)
		++openCLProgressCounter2;

	char progressChar;

	switch (openCLProgressCounter2 % 4)
	{
		case 1: progressChar = '\\';
			break;
		case 2: progressChar = '|';
			break;
		case 3: progressChar = '/';
			break;
		default: progressChar = '-';
			break;
	}

	printf("[%c] ", progressChar);
	printf("Elapsed time: %02d:%02d:%02d", elapsedHours, elapsedMinutes, elapsedSeconds);
	printf("          \r");
}
