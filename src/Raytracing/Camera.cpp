// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "glfw/glfw3.h"

#include "App/App.h"
#include "Raytracing/Camera.h"
#include "Raytracing/Ray.h"
#include "Math/MathUtils.h"

using namespace Raycer;

void Camera::setApp(App* app_)
{
	app = app_;
}

void Camera::setFov(float fov)
{
	imagePlaneDistance = 0.5f / tan(MathUtils::degToRad(fov / 2.0f));
}

void Camera::setImagePlaneSize(int width, int height)
{
	imagePlaneWidth = (float)(width - 1);
	imagePlaneHeight = (float)(height - 1);
	aspectRatio = (float)height / width;
}

void Camera::update(float timeStep)
{
	MouseInfo mouseInfo = app->getMouseInfo();

	if (app->mouseIsDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		orientation.yaw -= (float)mouseInfo.deltaX * timeStep * 5.0f;
		orientation.pitch += (float)mouseInfo.deltaY * timeStep * 5.0f;
	}

	forward = orientation.getDirectionVector().normalized();
	right = forward.cross(Vector3::UP).normalized();
	up = right.cross(forward).normalized();

	if (app->keyIsDown(GLFW_KEY_W))
		position += forward * timeStep * 5.0f;

	if (app->keyIsDown(GLFW_KEY_S))
		position -= forward * timeStep * 5.0f;

	if (app->keyIsDown(GLFW_KEY_A))
		position -= right * timeStep * 5.0f;

	if (app->keyIsDown(GLFW_KEY_D))
		position += right * timeStep * 5.0f;

	if (app->keyIsDown(GLFW_KEY_Q))
		position -= up * timeStep * 5.0f;

	if (app->keyIsDown(GLFW_KEY_E))
		position += up * timeStep * 5.0f;

	imagePlaneCenter = position + (forward * imagePlaneDistance);
}

Ray Camera::getRay(int x, int y) const
{
	float dx = ((float)x / imagePlaneWidth) - 0.5f;
	float dy = ((float)y / imagePlaneHeight) - 0.5f;

	Vector3 imagePlanePixelPosition = imagePlaneCenter + (dx * right) + (dy * aspectRatio * up);
	Vector3 rayDirection = (imagePlanePixelPosition - position).normalized();

	return Ray(position, rayDirection);
}
