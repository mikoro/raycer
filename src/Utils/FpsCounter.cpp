// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "tinyformat/tinyformat.h"

#include "Utils/FpsCounter.h"

using namespace Raycer;

FpsCounter::FpsCounter()
{
	lastTime = glfwGetTime();

	averageFrameTime.setAlpha(0.05);
	averageFrameTime.setAverage(1.0 / 30.0);
}

void FpsCounter::count()
{
	double currentTime = glfwGetTime();
	frameTime = currentTime - lastTime;
	lastTime = currentTime;

	// prevent too large frametime changes
	if (frameTime > 2 * averageFrameTime.getAverage())
		frameTime = 2 * averageFrameTime.getAverage();
}

void FpsCounter::update(double timeStep)
{
	(void)timeStep;

	averageFrameTime.addMeasurement(frameTime);
}

double FpsCounter::getFps() const
{
	return 1.0 / averageFrameTime.getAverage();
}

std::string FpsCounter::getFpsString() const
{
	return tfm::format("%.1f", getFps());
}
