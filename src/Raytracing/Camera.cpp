// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "Raytracing/Camera.h"
#include "App.h"
#include "Utils/Settings.h"
#include "Raytracing/Ray.h"
#include "Math/MathUtils.h"
#include "Math/Vector2.h"
#include "Runners/InteractiveRunner.h"

using namespace Raycer;

void Camera::initialize()
{
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

	double velocityLength = velocity.length();
	double angularVelocityLength = angularVelocity.length();

	if (velocityLength > 0.0)
		acceleration = settings.camera.moveDrag * (-velocity.normalized() * velocityLength);
	else
		acceleration = Vector3(0.0, 0.0, 0.0);
	
	if (angularVelocityLength > 0.0)
		angularAcceleration = settings.camera.mouseDrag * (-angularVelocity.normalized() * angularVelocityLength);
	else
		angularAcceleration = Vector3(0.0, 0.0, 0.0);

	MouseInfo mouseInfo = runner.getMouseInfo();

	if (runner.mouseIsDown(GLFW_MOUSE_BUTTON_LEFT) || settings.camera.freeLook)
	{
		angularAcceleration.y -= (double)mouseInfo.deltaX * settings.camera.mouseSpeed;
		angularAcceleration.x += (double)mouseInfo.deltaY * settings.camera.mouseSpeed;
	}

	double moveSpeed = settings.camera.moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_LEFT_CONTROL) || runner.keyIsDown(GLFW_KEY_RIGHT_CONTROL))
		moveSpeed *= settings.camera.slowModifier;

	if (runner.keyIsDown(GLFW_KEY_LEFT_SHIFT) || runner.keyIsDown(GLFW_KEY_RIGHT_SHIFT))
		moveSpeed *= settings.camera.fastModifier;

	if (runner.keyIsDown(GLFW_KEY_LEFT_ALT) || runner.keyIsDown(GLFW_KEY_RIGHT_ALT))
		moveSpeed *= settings.camera.veryFastModifier;

	if (runner.keyIsDown(GLFW_KEY_W) || runner.keyIsDown(GLFW_KEY_UP))
		acceleration += forward * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_S) || runner.keyIsDown(GLFW_KEY_DOWN))
		acceleration -= forward * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_A) || runner.keyIsDown(GLFW_KEY_LEFT))
		acceleration -= right * moveSpeed;
	
	if (runner.keyIsDown(GLFW_KEY_D) || runner.keyIsDown(GLFW_KEY_RIGHT))
		acceleration += right * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_Q))
		acceleration -= up * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_E))
		acceleration += up * moveSpeed;

	if (runner.keyIsDown(GLFW_KEY_PAGE_DOWN))
		fov -= 50.0 * timeStep;

	if (runner.keyIsDown(GLFW_KEY_PAGE_UP))
		fov += 50.0 * timeStep;

	fov = std::max(1.0, std::min(fov, 180.0));

	velocity += acceleration * timeStep;
	position += velocity * timeStep;

	angularVelocity += angularAcceleration * timeStep;
	orientation.yaw += angularVelocity.y * timeStep;
	orientation.pitch += angularVelocity.x * timeStep;

	orientation.clampPitch();
	orientation.normalize();
}

void Camera::precalculate()
{
	forward = orientation.getDirectionVector();
	right = forward.cross(Vector3::UP).normalized();
	up = right.cross(forward).normalized();

	imagePlaneDistance = 0.5 / tan(MathUtils::degToRad(fov / 2.0));
	imagePlaneCenter = position + (forward * imagePlaneDistance);
}

Ray Camera::getRay(const Vector2& pixel) const
{
	double dx = (pixel.x / imagePlaneWidth) - 0.5;
	double dy = (pixel.y / imagePlaneHeight) - 0.5;

	Vector3 imagePlanePixelPosition = imagePlaneCenter + (dx * right) + (dy * aspectRatio * up);

	Ray ray;
	ray.origin = position;
	ray.direction = (imagePlanePixelPosition - position).normalized();

	return ray;
}
