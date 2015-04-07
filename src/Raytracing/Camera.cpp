// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "glfw/glfw3.h"

#include "App/App.h"
#include "Raytracing/Camera.h"
#include "Raytracing/Ray.h"
#include "Math/MathUtils.h"

using namespace Raycer;

void Camera::initialize(App* app_, Settings* settings_)
{
	app = app_;
	settings = settings_;
}

void Camera::setImagePlaneSize(int width, int height)
{
	imagePlaneWidth = (double)(width - 1);
	imagePlaneHeight = (double)(height - 1);
	aspectRatio = (double)height / width;
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
	MouseInfo mouseInfo = app->getMouseInfo();

	if (app->mouseIsDown(GLFW_MOUSE_BUTTON_LEFT) || !settings->controls.mouseLookWithButton)
	{
		orientation.yaw -= (double)mouseInfo.deltaX * timeStep * settings->controls.mouseSpeed;
		orientation.pitch += (double)mouseInfo.deltaY * timeStep * settings->controls.mouseSpeed;
	}

	orientation.clampPitch();
	orientation.normalize();

	if (app->keyIsDown(GLFW_KEY_W) || app->keyIsDown(GLFW_KEY_UP))
		position += forward * timeStep * settings->controls.moveSpeed;

	if (app->keyIsDown(GLFW_KEY_S) || app->keyIsDown(GLFW_KEY_DOWN))
		position -= forward * timeStep * settings->controls.moveSpeed;

	if (app->keyIsDown(GLFW_KEY_A) || app->keyIsDown(GLFW_KEY_LEFT))
		position -= right * timeStep * settings->controls.moveSpeed;

	if (app->keyIsDown(GLFW_KEY_D) || app->keyIsDown(GLFW_KEY_RIGHT))
		position += right * timeStep * settings->controls.moveSpeed;

	if (app->keyIsDown(GLFW_KEY_Q))
		position -= up * timeStep * settings->controls.moveSpeed;

	if (app->keyIsDown(GLFW_KEY_E))
		position += up * timeStep * settings->controls.moveSpeed;

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
