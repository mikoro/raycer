// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "glfw/glfw3.h"
#include "tinyformat/tinyformat.h"

#include "Utils/FpsCounter.h"

using namespace Raycer;

FpsCounter::FpsCounter()
{
	lastTime = glfwGetTime();
}

void FpsCounter::countFrame()
{
	double currentTime = glfwGetTime();
	frameTime = currentTime - lastTime;
	lastTime = currentTime;

	// prevent too large frametime changes
	if (frameTime > 2 * averageFrameTime)
		frameTime = 2 * averageFrameTime;
}

void FpsCounter::update(double timeStep)
{
	(void)timeStep;

	double alpha = 0.05;
	averageFrameTime = alpha * frameTime + (1.0 - alpha) * averageFrameTime;
}

double FpsCounter::getFps() const
{
	return 1.0 / averageFrameTime;
}

std::string FpsCounter::getFpsString() const
{
	return tfm::format("%.1f", getFps());
}
