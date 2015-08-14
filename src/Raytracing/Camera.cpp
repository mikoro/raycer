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
#include "Raytracing/Ray.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Intersection.h"
#include "App.h"
#include "Utils/Settings.h"
#include "Math/MathUtils.h"
#include "Math/Vector2.h"
#include "Runners/WindowRunner.h"

using namespace Raycer;

Camera::Camera()
{
}

void Camera::initialize()
{
}

void Camera::setImagePlaneSize(int width, int height)
{
	imagePlaneWidth = (double)(width - 1);
	imagePlaneHeight = (double)(height - 1);
	aspectRatio = (double)height / (double)width;
}

void Camera::update(const Scene& scene, double timeStep)
{
	WindowRunner& windowRunner = App::getWindowRunner();
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

	MouseInfo mouseInfo = windowRunner.getMouseInfo();

	if (windowRunner.mouseIsDown(GLFW_MOUSE_BUTTON_LEFT) || settings.camera.freeLook)
	{
		angularAcceleration.y -= (double)mouseInfo.deltaX * settings.camera.mouseSpeed;
		angularAcceleration.x += (double)mouseInfo.deltaY * settings.camera.mouseSpeed;

		if (!settings.camera.freeLook)
			isMovingPrimitive = false;
	}

	if (windowRunner.mouseWasPressed(GLFW_MOUSE_BUTTON_RIGHT))
	{
		if (!isMovingPrimitive)
		{
			Vector2 pixelCoordinate;
			pixelCoordinate.x = mouseInfo.framebufferX;
			pixelCoordinate.y = mouseInfo.framebufferY;

			Ray ray = getRay(pixelCoordinate);
			Intersection intersection;

			for (Primitive* primitive : scene.primitives.all)
				primitive->intersect(ray, intersection);

			if (intersection.wasFound)
			{
				isMovingPrimitive = true;
				movingPrimitivePosition = intersection.primitive->getPosition();
			}
		}
		else
			isMovingPrimitive = false;
	}

	if (isMovingPrimitive && movingPrimitivePosition != nullptr)
	{
		if (windowRunner.mouseIsDown(GLFW_MOUSE_BUTTON_MIDDLE))
			*movingPrimitivePosition += Vector3(forward.x, 0.0, forward.z).normalized() * (double)mouseInfo.deltaY / 100.0;
		else
			*movingPrimitivePosition += Vector3::UP * (double)mouseInfo.deltaY / 100.0;

		*movingPrimitivePosition += right * (double)mouseInfo.deltaX / 100.0;
	}

	double moveSpeed = settings.camera.moveSpeed;

	if (windowRunner.keyIsDown(GLFW_KEY_LEFT_CONTROL) || windowRunner.keyIsDown(GLFW_KEY_RIGHT_CONTROL))
		moveSpeed *= settings.camera.slowModifier;

	if (windowRunner.keyIsDown(GLFW_KEY_LEFT_SHIFT) || windowRunner.keyIsDown(GLFW_KEY_RIGHT_SHIFT))
		moveSpeed *= settings.camera.fastModifier;

	if (windowRunner.keyIsDown(GLFW_KEY_LEFT_ALT) || windowRunner.keyIsDown(GLFW_KEY_RIGHT_ALT))
		moveSpeed *= settings.camera.veryFastModifier;

	if (windowRunner.keyIsDown(GLFW_KEY_W) || windowRunner.keyIsDown(GLFW_KEY_UP))
		acceleration += forward * moveSpeed;

	if (windowRunner.keyIsDown(GLFW_KEY_S) || windowRunner.keyIsDown(GLFW_KEY_DOWN))
		acceleration -= forward * moveSpeed;

	if (windowRunner.keyIsDown(GLFW_KEY_A) || windowRunner.keyIsDown(GLFW_KEY_LEFT))
		acceleration -= right * moveSpeed;

	if (windowRunner.keyIsDown(GLFW_KEY_D) || windowRunner.keyIsDown(GLFW_KEY_RIGHT))
		acceleration += right * moveSpeed;

	if (windowRunner.keyIsDown(GLFW_KEY_Q))
		acceleration -= up * moveSpeed;

	if (windowRunner.keyIsDown(GLFW_KEY_E))
		acceleration += up * moveSpeed;

	if (windowRunner.keyWasPressed(GLFW_KEY_HOME))
	{
		if (projectionType == CameraProjectionType::PERSPECTIVE)
			projectionType = CameraProjectionType::ORTHOGRAPHIC;
		else if (projectionType == CameraProjectionType::ORTHOGRAPHIC)
			projectionType = CameraProjectionType::FISHEYE;
		else if (projectionType == CameraProjectionType::FISHEYE)
			projectionType = CameraProjectionType::PERSPECTIVE;
	}

	if (windowRunner.keyIsDown(GLFW_KEY_PAGE_DOWN))
	{
		if (projectionType == CameraProjectionType::PERSPECTIVE)
			fov -= 50.0 * timeStep;
		else if (projectionType == CameraProjectionType::ORTHOGRAPHIC)
			orthoSize -= 10.0 * timeStep;
		else if (projectionType == CameraProjectionType::FISHEYE)
			fishEyeAngle -= 50.0 * timeStep;
	}

	if (windowRunner.keyIsDown(GLFW_KEY_PAGE_UP))
	{
		if (projectionType == CameraProjectionType::PERSPECTIVE)
			fov += 50.0 * timeStep;
		else if (projectionType == CameraProjectionType::ORTHOGRAPHIC)
			orthoSize += 10.0 * timeStep;
		else if (projectionType == CameraProjectionType::FISHEYE)
			fishEyeAngle += 50.0 * timeStep;
	}

	fov = std::max(1.0, std::min(fov, 180.0));
	orthoSize = std::max(0.0, orthoSize);
	fishEyeAngle = std::max(1.0, std::min(fishEyeAngle, 360.0));

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

Ray Camera::getRay(const Vector2& pixelCoordinate) const
{
	Ray ray;

	switch (projectionType)
	{
		case Raycer::CameraProjectionType::PERSPECTIVE:
		{
			double dx = (pixelCoordinate.x / imagePlaneWidth) - 0.5;
			double dy = (pixelCoordinate.y / imagePlaneHeight) - 0.5;

			Vector3 imagePlanePixelPosition = imagePlaneCenter + (dx * right) + (dy * aspectRatio * up);

			ray.origin = position;
			ray.direction = (imagePlanePixelPosition - position).normalized();

		} break;

		case Raycer::CameraProjectionType::ORTHOGRAPHIC:
		{
			double dx = (pixelCoordinate.x / imagePlaneWidth) - 0.5;
			double dy = (pixelCoordinate.y / imagePlaneHeight) - 0.5;

			ray.origin = position + (dx * orthoSize * right) + (dy * orthoSize * aspectRatio * up);
			ray.direction = forward;

		} break;

		// http://paulbourke.net/dome/fisheye/
		case Raycer::CameraProjectionType::FISHEYE:
		{
			double dx = (pixelCoordinate.x / imagePlaneWidth) * 2.0 - 1.0;
			double dy = (pixelCoordinate.y / imagePlaneHeight) * 2.0 - 1.0;

			dx /= std::min(1.0, aspectRatio);
			dy *= std::max(1.0, aspectRatio);

			double r = sqrt(dx * dx + dy * dy);

			if (r > 1.0)
			{
				ray.isInvalid = true;
				break;
			}

			double phi = atan2(dy, dx);
			double theta = r * (MathUtils::degToRad(fishEyeAngle) / 2.0);

			double u = sin(theta) * cos(phi);
			double v = sin(theta) * sin(phi);
			double w = cos(theta);

			ray.origin = position;
			ray.direction = u * right + v * up + w * forward;

		} break;

		default: break;
	}

	ray.update();
	return ray;
}
