// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "CpuRaytracing/Camera.h"
#include "App.h"
#include "Utils/Settings.h"
#include "CpuRaytracing/Ray.h"
#include "Math/MathUtils.h"
#include "Runners/InteractiveRunner.h"

using namespace Raycer;

void Camera::setImagePlaneSize(int width, int height)
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

	if (runner.mouseIsDown(GLFW_MOUSE_BUTTON_LEFT) || settings.controls.freeLook)
	{
		orientation.yaw -= (double)mouseInfo.deltaX * timeStep * settings.controls.mouseSpeed;
		orientation.pitch += (double)mouseInfo.deltaY * timeStep * settings.controls.mouseSpeed;
	}

	orientation.clampPitch();
	orientation.normalize();

	double moveSpeed = settings.controls.moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_LEFT_SHIFT) || runner.keyIsDown(GLFW_KEY_RIGHT_SHIFT))
		moveSpeed *= settings.controls.fastModifier;

	if (runner.keyIsDown(GLFW_KEY_LEFT_CONTROL) || runner.keyIsDown(GLFW_KEY_RIGHT_CONTROL))
		moveSpeed *= settings.controls.slowModifier;

	if (runner.keyIsDown(GLFW_KEY_W) || runner.keyIsDown(GLFW_KEY_UP))
		position += forward * timeStep * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_S) || runner.keyIsDown(GLFW_KEY_DOWN))
		position -= forward * timeStep * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_A) || runner.keyIsDown(GLFW_KEY_LEFT))
		position -= right * timeStep * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_D) || runner.keyIsDown(GLFW_KEY_RIGHT))
		position += right * timeStep * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_Q))
		position -= up * timeStep * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_E))
		position += up * timeStep * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_F7))
		fov -= 50.0 * timeStep;

	if (runner.keyIsDown(GLFW_KEY_F8))
		fov += 50.0 * timeStep;

	fov = std::max(1.0, std::min(fov, 180.0));

	calculateVariables();
}

Ray Camera::getRay(int x, int y) const
{
	double dx = ((double)x / imagePlaneWidth) - 0.5;
	double dy = ((double)y / imagePlaneHeight) - 0.5;

	Vector3 imagePlanePixelPosition = imagePlaneCenter + (dx * right) + (dy * aspectRatio * up);
	Vector3 rayDirection = (imagePlanePixelPosition - position).normalized();

	return Ray(position, rayDirection);
}
