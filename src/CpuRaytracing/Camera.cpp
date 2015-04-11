// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "glfw/glfw3.h"

#include "CpuRaytracing/Camera.h"
#include "App.h"
#include "Utils/Settings.h"
#include "CpuRaytracing/Ray.h"
#include "Math/MathUtils.h"
#include "Runners/InteractiveRunner.h"

using namespace Raycer;

void Camera::setImagePlaneSize(size_t width, size_t height)
{
	imagePlaneWidth = (double)(width - 1);
	imagePlaneHeight = (double)(height - 1);
	aspectRatio = (double)height / (double)width;
}

void Camera::calculateVariables()
{
	forward = orientation.getDirectionVector().normalized();
	right = forward.cross(Vector3::UP).normalized();
	up = right.cross(forward).normalized();

	imagePlaneDistance = 0.5 / tan(MathUtils::degToRad(fov / 2.0));
	imagePlaneCenter = position + (forward * imagePlaneDistance);
}

void Camera::update(double timeStep)
{
	InteractiveRunner& runner = App::getInteractiveRunner();
	Settings& settings = App::getSettings();

	MouseInfo mouseInfo = runner.getMouseInfo();

	if (runner.mouseIsDown(GLFW_MOUSE_BUTTON_LEFT) || !settings.controls.freeLook)
	{
		orientation.yaw -= (double)mouseInfo.deltaX * timeStep * settings.controls.mouseSpeed;
		orientation.pitch += (double)mouseInfo.deltaY * timeStep * settings.controls.mouseSpeed;
	}

	orientation.clampPitch();
	orientation.normalize();

	if (runner.keyIsDown(GLFW_KEY_W) || runner.keyIsDown(GLFW_KEY_UP))
		position += forward * timeStep * settings.controls.moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_S) || runner.keyIsDown(GLFW_KEY_DOWN))
		position -= forward * timeStep * settings.controls.moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_A) || runner.keyIsDown(GLFW_KEY_LEFT))
		position -= right * timeStep * settings.controls.moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_D) || runner.keyIsDown(GLFW_KEY_RIGHT))
		position += right * timeStep * settings.controls.moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_Q))
		position -= up * timeStep * settings.controls.moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_E))
		position += up * timeStep * settings.controls.moveSpeed;

	calculateVariables();
}

Ray Camera::getRay(size_t x, size_t y) const
{
	double dx = ((double)x / imagePlaneWidth) - 0.5;
	double dy = ((double)y / imagePlaneHeight) - 0.5;

	Vector3 imagePlanePixelPosition = imagePlaneCenter + (dx * right) + (dy * aspectRatio * up);
	Vector3 rayDirection = (imagePlanePixelPosition - position).normalized();

	return Ray(position, rayDirection);
}
