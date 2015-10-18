// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Runners/ConsoleRunner.h"
#include "App.h"
#include "Settings.h"
#include "Utils/StringUtils.h"
#include "Utils/SysUtils.h"
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

	pixelsPerSecondAverage.setAlpha(0.05);
	pixelsPerSecondAverage.setAverage(0.0);
	timer.setAveragingAlpha(0.05);
	progressCounter1 = progressCounter2 = 0;

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
			SysUtils::openFileExternally(settings.image.fileName);
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

	SysUtils::setConsoleTextColor(ConsoleTextColor::WHITE_ON_BLACK);

	std::cout << tfm::format("\nTracing started (dimensions: %dx%d, offset: %d, pixels: %d, size: %fB)\n\n",
		state.imageWidth,
		state.imageHeight,
		state.pixelStartOffset,
		state.pixelCount,
		StringUtils::humanizeNumber(double(state.pixelCount * sizeof(Color)), true));

	timer.setTargetValue(double(state.pixelCount));
	timer.restart();

	std::thread renderThread(renderThreadFunction);

	while (!renderThreadFinished)
	{
		timer.updateCurrentValue(double(state.pixelsProcessed));

		auto elapsed = timer.getElapsed();
		auto remaining = timer.getRemaining();

		if (elapsed.totalMilliseconds > 0)
			pixelsPerSecondAverage.addMeasurement(double(state.pixelsProcessed) / (double(elapsed.totalMilliseconds) / 1000.0));

		if (!settings.openCL.enabled)
			printProgress(elapsed, remaining);
		else
			printProgressOpenCL(elapsed, remaining);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	renderThread.join();

	if (renderThreadException != nullptr)
		std::rethrow_exception(renderThreadException);

	timer.updateCurrentValue(double(state.pixelsProcessed));

	auto elapsed = timer.getElapsed();
	auto remaining = timer.getRemaining();

	if (!settings.openCL.enabled)
		printProgress(elapsed, remaining);
	else
		printProgressOpenCL(elapsed, remaining);

	double totalPixelsPerSecond = 0.0;

	if (elapsed.totalMilliseconds > 0)
		totalPixelsPerSecond = double(state.pixelsProcessed) / (double(elapsed.totalMilliseconds) / 1000.0);

	std::cout << tfm::format("\n\nTracing %s (time: %s, pixels/s: %f)\n\n",
		interrupted ? "interrupted" : "finished",
		elapsed.getString(true),
		StringUtils::humanizeNumber(totalPixelsPerSecond));

	SysUtils::setConsoleTextColor(ConsoleTextColor::DEFAULT);

	if (!interrupted && settings.openCL.enabled)
		*state.toneMappedImage = clTracer.downloadImage();
}

void ConsoleRunner::interrupt()
{
	interrupted = true;
}

void ConsoleRunner::printProgress(const TimerData& elapsed, const TimerData& remaining)
{
	uint percentage = uint(timer.getPercentage() + 0.5);
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

	printf("] ");
	printf("%u %% | ", percentage);
	printf("Elapsed time: %s | ", elapsed.getString().c_str());
	printf("Remaining time: %s | ", remaining.getString().c_str());
	printf("Pixels/s: %s", StringUtils::humanizeNumber(pixelsPerSecondAverage.getAverage()).c_str());
	printf("          \r");
}

void ConsoleRunner::printProgressOpenCL(const TimerData& elapsed, const TimerData& remaining)
{
	if (++progressCounter1 % 5 == 0)
		++progressCounter2;

	char progressChar;

	switch (progressCounter2 % 4)
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
	printf("Elapsed time: %s | ", elapsed.getString().c_str());
	printf("Remaining time: %s | ", remaining.getString().c_str());
	printf("          \r");
}
