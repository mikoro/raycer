// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Runners/WindowRunnerStates/DefaultState.h"
#include "App.h"
#include "Settings.h"
#include "Utils/Log.h"
#include "OpenCL/CLTracer.h"
#include "Math/Color.h"
#include "Math/EulerAngle.h"
#include "Math/Vector3.h"
#include "Raytracing/Camera.h"
#include "Raytracing/Tracers/TracerState.h"
#include "Rendering/Text.h"
#include "Rendering/ImagePool.h"
#include "Runners/WindowRunner.h"

using namespace Raycer;

DefaultState::DefaultState() : interrupted(false) {}

void DefaultState::initialize()
{
	Settings& settings = App::getSettings();
	CLTracer& clTracer = App::getCLTracer();

	if (settings.scene.enableTestScenes)
		scene = Scene::createTestScene(settings.scene.testSceneNumber);
	else
		scene = Scene::loadFromFile(settings.scene.fileName);

	filmRenderer.initialize();
	windowResized(settings.window.width, settings.window.height);

	scene.initialize();
	tracer = Tracer::getTracer(scene.general.tracerType);

	if (settings.openCL.enabled)
	{
		clTracer.initializeKernels();
		clTracer.initializeBuffers(scene);
	}

	currentTestSceneNumber = settings.scene.testSceneNumber;
}

void DefaultState::pause()
{
}

void DefaultState::resume()
{
}

void DefaultState::shutdown()
{
}

void DefaultState::update(double timeStep)
{
	Log& log = App::getLog();
	Settings& settings = App::getSettings();
	WindowRunner& windowRunner = App::getWindowRunner();
	CLTracer& clTracer = App::getCLTracer();

	bool increaseTestSceneNumber = windowRunner.keyWasPressed(GLFW_KEY_F2);
	bool decreaseTestSceneNumber = windowRunner.keyWasPressed(GLFW_KEY_F3);

	if (increaseTestSceneNumber || decreaseTestSceneNumber)
	{
		uint64_t previousTestSceneNumber = currentTestSceneNumber;

		if (increaseTestSceneNumber)
			currentTestSceneNumber++;

		if (decreaseTestSceneNumber)
			currentTestSceneNumber--;

		if (currentTestSceneNumber < 1)
			currentTestSceneNumber = 1;

		if (currentTestSceneNumber > Scene::TEST_SCENE_COUNT)
			currentTestSceneNumber = Scene::TEST_SCENE_COUNT;

		if (previousTestSceneNumber != currentTestSceneNumber)
		{
			ImagePool::clear();

			try
			{
				scene = Scene::createTestScene(currentTestSceneNumber);
				scene.initialize();
			}
			catch (const std::exception& ex)
			{
				log.logWarning("Could not create test scene: %s", ex.what());

				scene = Scene();
				scene.initialize();
			}

			scene.camera.setImagePlaneSize(film.getWidth(), film.getHeight());
			tracer = Tracer::getTracer(scene.general.tracerType);

			if (settings.openCL.enabled)
				clTracer.initializeBuffers(scene);
		}
	}

	if (windowRunner.keyWasPressed(GLFW_KEY_R))
		scene.camera.reset();

	if (windowRunner.keyWasPressed(GLFW_KEY_N))
		scene.general.enableNormalMapping = !scene.general.enableNormalMapping;

	if (windowRunner.keyWasPressed(GLFW_KEY_B))
		scene.rebuildRootBVH();

	if (windowRunner.keyWasPressed(GLFW_KEY_F4))
	{
		if (scene.general.tracerType == TracerType::RAY)
			scene.general.tracerType = TracerType::PATH;
		else if (scene.general.tracerType == TracerType::PATH)
			scene.general.tracerType = TracerType::RAY;

		tracer = Tracer::getTracer(scene.general.tracerType);
		film.clear();
	}

	if (windowRunner.keyWasPressed(GLFW_KEY_F5))
	{
		if (scene.toneMapper.type == ToneMapperType::PASSTHROUGH)
		{
			scene.toneMapper.type = ToneMapperType::LINEAR;
			log.logInfo("Selected linear tone mapper");
		}
		else if (scene.toneMapper.type == ToneMapperType::LINEAR)
		{
			scene.toneMapper.type = ToneMapperType::SIMPLE;
			log.logInfo("Selected simple tone mapper");
		}
		else if (scene.toneMapper.type == ToneMapperType::SIMPLE)
		{
			scene.toneMapper.type = ToneMapperType::REINHARD;
			log.logInfo("Selected reinhard tone mapper");
		}
		else if (scene.toneMapper.type == ToneMapperType::REINHARD)
		{
			scene.toneMapper.type = ToneMapperType::PASSTHROUGH;
			log.logInfo("Selected passthrough tone mapper");
		}
	}

	if (windowRunner.keyWasPressed(GLFW_KEY_F6))
		scene.general.visualizeDepth = !scene.general.visualizeDepth;

	if (windowRunner.keyWasPressed(GLFW_KEY_F7))
	{
		if (windowRunner.keyIsDown(GLFW_KEY_LEFT_CONTROL) || windowRunner.keyIsDown(GLFW_KEY_RIGHT_CONTROL))
			scene.saveToFile("scene.xml");
		else
			scene.saveToFile("scene.bin");
	}

	if (windowRunner.keyWasPressed(GLFW_KEY_F8))
	{
		windowRunner.pause();
		scene.saveToFile("scene.bin");

#ifdef _WIN32
		ShellExecuteA(nullptr, "open", "raycer.exe", "-s scene.bin --non-interactive --non-test --view", nullptr, SW_SHOWNORMAL);
#else
		int32_t pid = fork();

		if (pid == 0)
		{
			char* arg[] = { (char*)"raycer", (char*)"-s scene.bin", (char*)"--non-interactive", (char*)"--non-test", (char*)"--view", (char*)nullptr };

			if (execvp(arg[0], arg) == -1)
				App::getLog().logWarning("Could not launch external rendering (%d) (try adding raycer to PATH)", errno);
		}
#endif
	}

	if (windowRunner.keyWasPressed(GLFW_KEY_F10))
	{
		double newScale = settings.framebuffer.scale * 0.5;
		uint64_t newWidth = uint64_t(double(windowRunner.getWindowWidth()) * newScale + 0.5);
		uint64_t newHeight = uint64_t(double(windowRunner.getWindowHeight()) * newScale + 0.5);

		if (newWidth >= 2 && newHeight >= 2)
		{
			settings.framebuffer.scale = newScale;
			resizeFilm();
		}
	}

	if (windowRunner.keyWasPressed(GLFW_KEY_F11))
	{
		if (settings.framebuffer.scale < 1.0)
		{
			settings.framebuffer.scale *= 2.0;

			if (settings.framebuffer.scale > 1.0)
				settings.framebuffer.scale = 1.0;

			resizeFilm();
		}
	}

	if (windowRunner.keyIsDown(GLFW_KEY_LEFT_CONTROL) || windowRunner.keyIsDown(GLFW_KEY_RIGHT_CONTROL))
	{
		if (windowRunner.keyIsDown(GLFW_KEY_PAGE_DOWN))
		{
			scene.toneMapper.exposure -= 2.0 * timeStep;
			scene.toneMapper.key -= 0.5 * timeStep;
		}
		else if (windowRunner.keyIsDown(GLFW_KEY_PAGE_UP))
		{
			scene.toneMapper.exposure += 2.0 * timeStep;
			scene.toneMapper.key += 0.5 * timeStep;
		}

		scene.toneMapper.key = std::max(0.0, scene.toneMapper.key);
	}

	scene.camera.update(scene, timeStep);
}

void DefaultState::render(double timeStep, double interpolation)
{
	(void)timeStep;
	(void)interpolation;

	Settings& settings = App::getSettings();
	WindowRunner& windowRunner = App::getWindowRunner();
	CLTracer& clTracer = App::getCLTracer();
	Text& text = windowRunner.getDefaultText();

	TracerState state;
	state.scene = &scene;
	state.film = &film;
	state.filmWidth = film.getWidth();
	state.filmHeight = film.getHeight();
	state.pixelStartOffset = 0;
	state.pixelCount = state.filmWidth * state.filmHeight;

	if (scene.general.tracerType == TracerType::RAY || (scene.general.tracerType == TracerType::PATH && scene.camera.hasMoved()))
	{
		film.clear();
		sampleCount = 0;
	}

	++sampleCount;

	if (!settings.openCL.enabled)
	{
		tracer->run(state, interrupted);
		film.generateToneMappedImage(scene);
		filmRenderer.uploadFilmData(film);
	}
	else
		clTracer.run(state, interrupted);

	filmRenderer.render();

	if (settings.window.showInfoText)
	{
		int64_t scaledMouseX = windowRunner.getMouseInfo().scaledX;
		int64_t scaledMouseY = windowRunner.getMouseInfo().scaledY;
		int64_t scaledMouseIndex = scaledMouseY * film.getWidth() + scaledMouseX;

		text.drawText(5.0, double(windowRunner.getWindowHeight() - 3 * settings.window.defaultFontSize), Color(255, 255, 255, 255), tfm::format("Pos: (%.2f, %.2f, %.2f)", scene.camera.position.x, scene.camera.position.y, scene.camera.position.z));
		text.drawText(5.0, double(windowRunner.getWindowHeight() - 4 * settings.window.defaultFontSize - 2), Color(255, 255, 255, 255), tfm::format("Rot: (%.2f, %.2f, %.2f)", scene.camera.orientation.pitch, scene.camera.orientation.yaw, scene.camera.orientation.roll));
		text.drawText(5.0, double(windowRunner.getWindowHeight() - 5 * settings.window.defaultFontSize - 4), Color(255, 255, 255, 255), tfm::format("Pix: (%d, %d, %d)", scaledMouseX, scaledMouseY, scaledMouseIndex));
		text.drawText(5.0, double(windowRunner.getWindowHeight() - 6 * settings.window.defaultFontSize - 6), Color(255, 255, 255, 255), tfm::format("Mov: %s", scene.camera.hasMoved()));

		if (scene.general.tracerType == TracerType::PATH)
			text.drawText(5.0, double(windowRunner.getWindowHeight() - 7 * settings.window.defaultFontSize - 8), Color(255, 255, 255, 255), tfm::format("Sam: %s", sampleCount));
	}
}

void DefaultState::windowResized(uint64_t width, uint64_t height)
{
	filmRenderer.setWindowSize(width, height);
	resizeFilm();
}

void DefaultState::resizeFilm()
{
	Settings& settings = App::getSettings();
	WindowRunner& windowRunner = App::getWindowRunner();
	CLTracer& clTracer = App::getCLTracer();

	uint64_t filmWidth = uint64_t(double(windowRunner.getWindowWidth()) * settings.framebuffer.scale + 0.5);
	uint64_t filmHeight = uint64_t(double(windowRunner.getWindowHeight()) * settings.framebuffer.scale + 0.5);

    filmWidth = std::max(uint64_t(1), filmWidth);
    filmHeight = std::max(uint64_t(1), filmHeight);

	if (settings.openCL.enabled)
		clTracer.releaseImageBuffer();

	film.resize(filmWidth, filmHeight);

	if (settings.openCL.enabled)
		clTracer.initializeImageBuffer(filmWidth, filmHeight, filmRenderer.getFilmTextureId());

	filmRenderer.setFilmSize(filmWidth, filmHeight);
	scene.camera.setImagePlaneSize(filmWidth, filmHeight);
}
