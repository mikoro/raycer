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
	MouseInfo mouseInfo = windowRunner.getMouseInfo();

	// PRIMITIVE SELECTION AND MOVEMENT //

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
				movingPrimitive = intersection.primitive;
			}
		}
		else
			isMovingPrimitive = false;
	}

	if (isMovingPrimitive && movingPrimitive != nullptr)
	{
		Vector3 scale(1.0, 1.0, 1.0);
		EulerAngle rotate(0.0, 0.0, 0.0);
		Vector3 translate(0.0, 0.0, 0.0);

		if (windowRunner.keyIsDown(GLFW_KEY_SPACE))
		{
			rotate.pitch -= (double)mouseInfo.deltaY / 5.0;

			if (windowRunner.mouseIsDown(GLFW_MOUSE_BUTTON_MIDDLE))
				rotate.roll -= (double)mouseInfo.deltaX / 5.0;
			else
				rotate.yaw += (double)mouseInfo.deltaX / 5.0;
		}
		else
		{
			if (windowRunner.mouseIsDown(GLFW_MOUSE_BUTTON_MIDDLE))
				translate += Vector3(forward.x, 0.0, forward.z).normalized() * (double)mouseInfo.deltaY / 100.0;
			else
				translate += Vector3::UP * (double)mouseInfo.deltaY / 100.0;

			translate += right * (double)mouseInfo.deltaX / 100.0;
		}

		scale *= (1.0 + windowRunner.getMouseWheelScroll() * 0.01);
		movingPrimitive->transform(scale, rotate, translate);
	}

	// LENS STUFF //

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

	// CAMERA MOVEMENT //

	double smoothVelocityLength = smoothVelocity.length();
	double smoothAngularVelocityLength = smoothAngularVelocity.length();

	if (smoothVelocityLength > 0.01)
		smoothAcceleration = settings.camera.moveDrag * (-smoothVelocity.normalized() * smoothVelocityLength);
	else
	{
		smoothVelocity = Vector3(0.0, 0.0, 0.0);
		smoothAcceleration = Vector3(0.0, 0.0, 0.0);
	}

	if (smoothAngularVelocityLength > 0.01)
		smoothAngularAcceleration = settings.camera.mouseDrag * (-smoothAngularVelocity.normalized() * smoothAngularVelocityLength);
	else
	{
		smoothAngularVelocity = Vector3(0.0, 0.0, 0.0);
		smoothAngularAcceleration = Vector3(0.0, 0.0, 0.0);
	}

	velocity = Vector3(0.0, 0.0, 0.0);
	angularVelocity = Vector3(0.0, 0.0, 0.0);

	if (windowRunner.mouseIsDown(GLFW_MOUSE_BUTTON_LEFT) || settings.camera.freeLook)
	{
		smoothAngularAcceleration.y -= (double)mouseInfo.deltaX * settings.camera.mouseSpeed;
		smoothAngularAcceleration.x += (double)mouseInfo.deltaY * settings.camera.mouseSpeed;
		angularVelocity.y = -(double)mouseInfo.deltaX * settings.camera.mouseSpeed;
		angularVelocity.x = (double)mouseInfo.deltaY * settings.camera.mouseSpeed;

		if (!settings.camera.freeLook)
			isMovingPrimitive = false;
	}

	double moveSpeed = settings.camera.moveSpeed;

	if (windowRunner.keyIsDown(GLFW_KEY_LEFT_CONTROL) || windowRunner.keyIsDown(GLFW_KEY_RIGHT_CONTROL))
		moveSpeed *= settings.camera.slowModifier;

	if (windowRunner.keyIsDown(GLFW_KEY_LEFT_SHIFT) || windowRunner.keyIsDown(GLFW_KEY_RIGHT_SHIFT))
		moveSpeed *= settings.camera.fastModifier;

	if (windowRunner.keyIsDown(GLFW_KEY_LEFT_ALT) || windowRunner.keyIsDown(GLFW_KEY_RIGHT_ALT))
		moveSpeed *= settings.camera.veryFastModifier;

	if (windowRunner.keyIsDown(GLFW_KEY_W) || windowRunner.keyIsDown(GLFW_KEY_UP))
	{
		smoothAcceleration += forward * moveSpeed;
		velocity += forward * moveSpeed;
	}

	if (windowRunner.keyIsDown(GLFW_KEY_S) || windowRunner.keyIsDown(GLFW_KEY_DOWN))
	{
		smoothAcceleration -= forward * moveSpeed;
		velocity -= forward * moveSpeed;
	}

	if (windowRunner.keyIsDown(GLFW_KEY_A) || windowRunner.keyIsDown(GLFW_KEY_LEFT))
	{
		smoothAcceleration -= right * moveSpeed;
		velocity -= right * moveSpeed;
	}

	if (windowRunner.keyIsDown(GLFW_KEY_D) || windowRunner.keyIsDown(GLFW_KEY_RIGHT))
	{
		smoothAcceleration += right * moveSpeed;
		velocity += right * moveSpeed;
	}

	if (windowRunner.keyIsDown(GLFW_KEY_Q))
	{
		smoothAcceleration -= up * moveSpeed;
		velocity -= up * moveSpeed;
	}

	if (windowRunner.keyIsDown(GLFW_KEY_E))
	{
		smoothAcceleration += up * moveSpeed;
		velocity += up * moveSpeed;
	}

	cameraHasMoved = false;

	if (settings.camera.smoothMovement)
	{
		smoothVelocity += smoothAcceleration * timeStep;
		position += smoothVelocity * timeStep;

		smoothAngularVelocity += smoothAngularAcceleration * timeStep;
		orientation.yaw += smoothAngularVelocity.y * timeStep;
		orientation.pitch += smoothAngularVelocity.x * timeStep;

		if (!smoothVelocity.isZero() || !smoothAngularVelocity.isZero())
			cameraHasMoved = true;
	}
	else
	{
		position += velocity * timeStep;
		orientation.yaw += angularVelocity.y * timeStep;
		orientation.pitch += angularVelocity.x * timeStep;

		if (!velocity.isZero() || !angularVelocity.isZero())
			cameraHasMoved = true;
	}
	
	orientation.clampPitch();
	orientation.normalize();
}

void Camera::precalculate()
{
	forward = orientation.getDirection();
	right = forward.cross(Vector3::ALMOST_UP).normalized();
	up = right.cross(forward).normalized();

	imagePlaneDistance = 0.5 / tan(MathUtils::degToRad(fov / 2.0));
	imagePlaneCenter = position + (forward * imagePlaneDistance);
}

bool Camera::hasMoved() const
{
	return cameraHasMoved;
}

Ray Camera::getRay(const Vector2& pixelCoordinate) const
{
	Ray ray;

	switch (projectionType)
	{
		case CameraProjectionType::PERSPECTIVE:
		{
			double dx = (pixelCoordinate.x / imagePlaneWidth) - 0.5;
			double dy = (pixelCoordinate.y / imagePlaneHeight) - 0.5;

			Vector3 imagePlanePixelPosition = imagePlaneCenter + (dx * right) + (dy * aspectRatio * up);

			ray.origin = position;
			ray.direction = (imagePlanePixelPosition - position).normalized();

		} break;

		case CameraProjectionType::ORTHOGRAPHIC:
		{
			double dx = (pixelCoordinate.x / imagePlaneWidth) - 0.5;
			double dy = (pixelCoordinate.y / imagePlaneHeight) - 0.5;

			ray.origin = position + (dx * orthoSize * right) + (dy * orthoSize * aspectRatio * up);
			ray.direction = forward;

		} break;

		// http://paulbourke.net/dome/fisheye/
		case CameraProjectionType::FISHEYE:
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
