// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <atomic>
#include <cstdio>
#include <iostream>
#include <ratio>
#include <thread>
#include <type_traits>

#ifdef _WIN32
#include <windows.h>
#endif

#include "tinyformat/tinyformat.h"

#include "Runners/ConsoleRunner.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "OpenCL/CLManager.h"
#include "Rendering/Image.h"
#include "Rendering/ToneMapper.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Raytracer.h"
#include "Raytracing/RaytracerState.h"
#include "Raytracing/Camera.h"
#include "OpenCL/CLRaytracer.h"

using namespace Raycer;
using namespace std::chrono;

int ConsoleRunner::run()
{
	Log& log = App::getLog();
	Settings& settings = App::getSettings();

	interrupted = false;

	pixelsPerSecondAverage.setAlpha(0.1);
	pixelsPerSecondAverage.setAverage(1.0);
	remainingTimeAverage.setAlpha(0.1);
	remainingTimeAverage.setAverage(0.0);

	Scene scene;
	
	if (settings.scene.enableTestScene)
		scene = Scene::createTestScene(settings.scene.testSceneNumber);
	else
		scene = Scene::loadFromFile(settings.scene.fileName);

	scene.initialize();
	scene.camera.setImagePlaneSize(settings.image.width, settings.image.height);
	scene.camera.precalculate();

	RaytracerState state;
	state.image = &image;
	state.scene = &scene;
	state.sceneWidth = settings.image.width;
	state.sceneHeight = settings.image.height;
	state.pixelOffset = 0;
	state.pixelCount = state.sceneWidth * state.sceneHeight;

	image.setSize(state.sceneWidth, state.sceneHeight);

	run(state);

	if (!interrupted)
	{
		image.saveAs(settings.image.fileName);

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

void ConsoleRunner::run(RaytracerState& state)
{
	Settings& settings = App::getSettings();
	CLManager& clManager = App::getCLManager();
	Raytracer& raytracer = App::getRaytracer();
	CLRaytracer& clRaytracer = App::getCLRaytracer();

	interrupted = false;

	if (settings.openCL.enabled && !openCLInitialized)
	{
		clManager.initialize();
		clManager.loadKernels();

		openCLInitialized = true;
	}

	if (settings.openCL.enabled)
	{
		clRaytracer.initialize();
		clRaytracer.resizePixelBuffer(state.sceneWidth, state.sceneHeight);
	}
	
	std::atomic<bool> finished;
	finished = false;

	auto renderFunction = [&]()
	{
		if (!settings.openCL.enabled)
			raytracer.run(state, interrupted);
		else
			clRaytracer.run(state, interrupted);

		if (state.scene->toneMapper.enabled)
		{
			switch (state.scene->toneMapper.type)
			{
				case ToneMapType::GAMMA: ToneMapper::gamma(*state.image, state.scene->toneMapper.gamma); break;
				case ToneMapType::REINHARD: break;
				default: break;
			}
		}
		
		finished = true;
	};

	std::cout << tfm::format("\nStart raytracing (dimensions: %dx%d, pixels: %s, size: %s, offset: %d)\n\n", state.sceneWidth, state.sceneHeight, humanizeNumberDecimal(state.pixelCount), humanizeNumberBytes(state.pixelCount * 4 * 4), state.pixelOffset);

	auto startTime = high_resolution_clock::now();
	std::thread renderThread(renderFunction);

	while (!finished)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (!settings.openCL.enabled)
			printProgress(startTime, state.pixelCount, state.pixelsProcessed);
		else
			printProgressOpenCL(startTime);
	}

	renderThread.join();

	if (!settings.openCL.enabled)
		printProgress(startTime, state.pixelCount, state.pixelsProcessed);
	else
		printProgressOpenCL(startTime);

	if (settings.openCL.enabled)
		state.pixelsProcessed = state.pixelCount;

	auto elapsedTime = high_resolution_clock::now() - startTime;
	int totalElapsedSeconds = (int)duration_cast<std::chrono::seconds>(elapsedTime).count();
	int totalElapsedMilliseconds = (int)duration_cast<std::chrono::milliseconds>(elapsedTime).count();
	int elapsedHours = totalElapsedSeconds / 3600;
	int elapsedMinutes = (totalElapsedSeconds - elapsedHours * 3600) / 60;
	int elapsedSeconds = totalElapsedSeconds - elapsedHours * 3600 - elapsedMinutes * 60;
	int elapsedMilliseconds = totalElapsedMilliseconds - totalElapsedSeconds * 1000;

	double totalPixelsPerSecond = 0.0;

	if (totalElapsedMilliseconds > 0)
		totalPixelsPerSecond = (double)state.pixelsProcessed / ((double)totalElapsedMilliseconds / 1000.0);
	
	std::string timeString = tfm::format("%02d:%02d:%02d.%03d", elapsedHours, elapsedMinutes, elapsedSeconds, elapsedMilliseconds);
	std::cout << tfm::format("\n\nRaytracing %s (time: %s, pixels: %s, pixels/s: %s)\n\n", interrupted ? "interrupted" : "finished", timeString, humanizeNumberDecimal(state.pixelsProcessed.load()), humanizeNumberDecimal(totalPixelsPerSecond));
	
	if (!interrupted && settings.openCL.enabled)
		image = clRaytracer.getImage();
}

void ConsoleRunner::interrupt()
{
	interrupted = true;
}

Image& ConsoleRunner::getResultImage()
{
	return image;
}

void ConsoleRunner::printProgress(const time_point<high_resolution_clock>& startTime, int totalPixelCount, int pixelsProcessed)
{
	auto elapsedTime = high_resolution_clock::now() - startTime;
	double elapsedSeconds = (double)duration_cast<std::chrono::milliseconds>(elapsedTime).count() / 1000.0;
	double msPerPixel = 0.0;

	if (pixelsProcessed > 0)
		msPerPixel = (double)duration_cast<std::chrono::milliseconds>(elapsedTime).count() / (double)pixelsProcessed;

	auto estimatedTime = std::chrono::milliseconds((int)(msPerPixel * (double)totalPixelCount + 0.5));
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

	if (elapsedSeconds > 0.0)
		pixelsPerSecondAverage.addMeasurement((double)pixelsProcessed / elapsedSeconds);
	
	remainingTimeAverage.addMeasurement((double)duration_cast<std::chrono::seconds>(remainingTime).count());

	if (pixelsProcessed == totalPixelCount)
		remainingTimeAverage.setAverage(0.0);

	int totalRemainingSeconds = (int)(remainingTimeAverage.getAverage() + 0.5);
	int remainingHours = totalRemainingSeconds / 3600;
	int remainingMinutes = (totalRemainingSeconds - remainingHours * 3600) / 60;
	int remainingSeconds = totalRemainingSeconds - remainingHours * 3600 - remainingMinutes * 60;

	printf("] ");
	printf("%d %% | ", percentage);
	printf("Remaining time: %02d:%02d:%02d | ", remainingHours, remainingMinutes, remainingSeconds);
	printf("Pixels/s: %s", humanizeNumberDecimal(pixelsPerSecondAverage.getAverage()).c_str());
	printf("          \r");
}

void ConsoleRunner::printProgressOpenCL(const std::chrono::time_point<std::chrono::high_resolution_clock>& startTime)
{
	auto elapsedTime = high_resolution_clock::now() - startTime;
	int totalElapsedSeconds = (int)duration_cast<std::chrono::seconds>(elapsedTime).count();
	int elapsedHours = totalElapsedSeconds / 3600;
	int elapsedMinutes = (totalElapsedSeconds - elapsedHours * 3600) / 60;
	int elapsedSeconds = totalElapsedSeconds - elapsedHours * 3600 - elapsedMinutes * 60;

	if (++openCLProgressCounter1 % 5 == 0)
		++openCLProgressCounter2;

	char progressChar;

	switch (openCLProgressCounter2 % 4)
	{
		case 1: progressChar = '\\'; break;
		case 2: progressChar = '|'; break;
		case 3: progressChar = '/'; break;
		default: progressChar = '-'; break;
	}

	printf("[%c] ", progressChar);
	printf("Elapsed time: %02d:%02d:%02d", elapsedHours, elapsedMinutes, elapsedSeconds);
	printf("          \r");
}

std::string ConsoleRunner::humanizeNumberDecimal(double value)
{
	const char* prefixes[] = {"", "k", "M", "G", "T", "P", "E", "Z", "Y"};

	for (int i = 0; i < 9; ++i)
	{
		if (value < 1000.0)
			return tfm::format("%.2f %s", value, prefixes[i]);
		else
			value /= 1000.0;
	}

	return tfm::format("%.2f %s", value, "Y");
}

std::string ConsoleRunner::humanizeNumberBytes(double value)
{
	const char* prefixes[] = { "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };

	for (int i = 0; i < 9; ++i)
	{
		if (value < 1024.0)
			return tfm::format("%.2f %s", value, prefixes[i]);
		else
			value /= 1024.0;
	}

	return tfm::format("%.2f %s", value, "YB");
}
