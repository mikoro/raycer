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
#include "Rendering/Framebuffer.h"
#include "Rendering/Text.h"
#include "Rendering/ImagePool.h"
#include "Runners/WindowRunner.h"

using namespace Raycer;

Raycer::DefaultState::DefaultState() : interrupted(false)
{
}

void DefaultState::initialize()
{
	Settings& settings = App::getSettings();
	Framebuffer& framebuffer = App::getFramebuffer();
	CLTracer& clTracer = App::getCLTracer();

	if (settings.scene.enableTestScenes)
		scene = Scene::createTestScene(settings.scene.testSceneNumber);
	else
		scene = Scene::loadFromFile(settings.scene.fileName);

	scene.initialize();
	scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());
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
	Framebuffer& framebuffer = App::getFramebuffer();
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

			scene.camera.setImagePlaneSize(framebuffer.getWidth(), framebuffer.getHeight());
			tracer = Tracer::getTracer(scene.general.tracerType);

			framebuffer.clear();
			state.cumulativeSampleCount = 0;

			if (settings.openCL.enabled)
				clTracer.initializeBuffers(scene);
		}
	}

	if (windowRunner.keyWasPressed(GLFW_KEY_R))
		scene.camera.reset();

	if (windowRunner.keyWasPressed(GLFW_KEY_F4))
	{
		if (scene.general.tracerType == TracerType::WHITTED)
			scene.general.tracerType = TracerType::PATH;
		else if (scene.general.tracerType == TracerType::PATH)
			scene.general.tracerType = TracerType::WHITTED;

		tracer = Tracer::getTracer(scene.general.tracerType);
		
		framebuffer.clear();
		state.cumulativeSampleCount = 0;
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

	Framebuffer& framebuffer = App::getFramebuffer();
	Settings& settings = App::getSettings();
	CLTracer& clTracer = App::getCLTracer();
	WindowRunner& runner = App::getWindowRunner();
	Text& text = runner.getDefaultText();

	if (scene.general.tracerType == TracerType::PATH && scene.camera.hasMoved())
	{
		framebuffer.clear();
		state.cumulativeSampleCount = 0;
	}

	state.scene = &scene;
	state.cumulativeImage = &framebuffer.getCumulativeImage();
	state.linearImage = &framebuffer.getLinearImage();
	state.toneMappedImage = &framebuffer.getToneMappedImage();
	state.imageWidth = framebuffer.getWidth();
	state.imageHeight = framebuffer.getHeight();
	state.pixelStartOffset = 0;
	state.pixelCount = state.imageWidth * state.imageHeight;
	state.pixelsProcessed = 0;

	if (!settings.openCL.enabled)
		tracer->run(state, interrupted);
	else
		clTracer.run(state, interrupted);

	if (settings.window.showInfoText)
	{
		text.drawText(5.0, double(runner.getWindowHeight() - 3 * settings.window.defaultFontSize), Color(255, 255, 255, 255), tfm::format("Pos: (%.2f, %.2f, %.2f)", scene.camera.position.x, scene.camera.position.y, scene.camera.position.z));
		text.drawText(5.0, double(runner.getWindowHeight() - 4 * settings.window.defaultFontSize - 2), Color(255, 255, 255, 255), tfm::format("Rot: (%.2f, %.2f, %.2f)", scene.camera.orientation.pitch, scene.camera.orientation.yaw, scene.camera.orientation.roll));
		text.drawText(5.0, double(runner.getWindowHeight() - 5 * settings.window.defaultFontSize - 4), Color(255, 255, 255, 255), tfm::format("Pix: (%d, %d)", runner.getMouseInfo().framebufferX, runner.getMouseInfo().framebufferY));
		text.drawText(5.0, double(runner.getWindowHeight() - 6 * settings.window.defaultFontSize - 6), Color(255, 255, 255, 255), tfm::format("Mov: %s", scene.camera.hasMoved()));
		text.drawText(5.0, double(runner.getWindowHeight() - 7 * settings.window.defaultFontSize - 8), Color(255, 255, 255, 255), tfm::format("Pat: %s", state.cumulativeSampleCount));
	}
}

void DefaultState::windowResized(uint64_t width, uint64_t height)
{
	(void)width;
	(void)height;
}

void DefaultState::framebufferResized(uint64_t width, uint64_t height)
{
	Framebuffer& framebuffer = App::getFramebuffer();

	scene.camera.setImagePlaneSize(width, height);

	framebuffer.clear();
	state.cumulativeSampleCount = 0;
}
