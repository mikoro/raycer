// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Raytracing/Camera.h"
#include "App.h"
#include "Utils/Settings.h"
#include "Raytracing/Ray.h"
#include "Math/MathUtils.h"
#include "Runners/InteractiveRunner.h"

using namespace Raycer;

void Camera::initialize()
{
	oldPosition = newPosition = position;
	oldOrientation = newOrientation = orientation;
}

void Camera::setImagePlaneSize(int width, int height)
{
	imagePlaneWidth = (double)(width - 1);
	imagePlaneHeight = (double)(height - 1);
	aspectRatio = (double)height / (double)width;
}

void Camera::update(double timeStep)
{
	InteractiveRunner& runner = App::getInteractiveRunner();
	Settings& settings = App::getSettings();

	oldPosition = newPosition;
	oldOrientation = newOrientation;

	MouseInfo mouseInfo = runner.getMouseInfo();

	if (runner.mouseIsDown(GLFW_MOUSE_BUTTON_LEFT) || settings.controls.freeLook)
	{
		newOrientation.yaw -= (double)mouseInfo.deltaX * timeStep * settings.controls.mouseSpeed;
		newOrientation.pitch += (double)mouseInfo.deltaY * timeStep * settings.controls.mouseSpeed;
	}

	newOrientation.clampPitch();
	// newOrientation.normalize(); // breaks the yaw interpolation

	double moveSpeed = settings.controls.moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_LEFT_SHIFT) || runner.keyIsDown(GLFW_KEY_RIGHT_SHIFT))
		moveSpeed *= settings.controls.fastModifier;

	if (runner.keyIsDown(GLFW_KEY_LEFT_CONTROL) || runner.keyIsDown(GLFW_KEY_RIGHT_CONTROL))
		moveSpeed *= settings.controls.slowModifier;

	if (runner.keyIsDown(GLFW_KEY_W) || runner.keyIsDown(GLFW_KEY_UP))
		newPosition += forward * timeStep * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_S) || runner.keyIsDown(GLFW_KEY_DOWN))
		newPosition -= forward * timeStep * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_A) || runner.keyIsDown(GLFW_KEY_LEFT))
		newPosition -= right * timeStep * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_D) || runner.keyIsDown(GLFW_KEY_RIGHT))
		newPosition += right * timeStep * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_Q))
		newPosition -= up * timeStep * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_E))
		newPosition += up * timeStep * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_F7))
		fov -= 50.0 * timeStep;

	if (runner.keyIsDown(GLFW_KEY_F8))
		fov += 50.0 * timeStep;

	fov = std::max(1.0, std::min(fov, 180.0));
}

void Camera::interpolate(double interpolation)
{
	position = Vector3::lerp(oldPosition, newPosition, interpolation);
	orientation = EulerAngle::lerp(oldOrientation, newOrientation, interpolation);
}

void Camera::precalculate()
{
	forward = orientation.getDirectionVector().normalized();
	right = forward.cross(Vector3::UP).normalized();
	up = right.cross(forward).normalized();

	imagePlaneDistance = 0.5 / tan(MathUtils::degToRad(fov / 2.0));
	imagePlaneCenter = position + (forward * imagePlaneDistance);
}

Ray Camera::getRay(int x, int y) const
{
	double dx = ((double)x / imagePlaneWidth) - 0.5;
	double dy = ((double)y / imagePlaneHeight) - 0.5;

	Vector3 imagePlanePixelPosition = imagePlaneCenter + (dx * right) + (dy * aspectRatio * up);
	Vector3 rayDirection = (imagePlanePixelPosition - position).normalized();

	return Ray(position, rayDirection);
}
