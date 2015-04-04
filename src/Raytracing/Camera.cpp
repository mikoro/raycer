// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cmath>

#include "Raytracing/Camera.h"
#include "Raytracing/Ray.h"
#include "Math/MathUtils.h"

using namespace Raycer;

Camera::Camera()
{
}

void Camera::setFov(float fov)
{
	imagePlaneDistance = 0.5f / tan(MathUtils::degToRad(fov));
}

void Camera::setSize(int width_, int height_)
{
	//aspectRatio = (float)height / width;
	width = width_;
	height = height_;
}

Ray Camera::getRay(int x, int y) const
{
	Vector3 cameraForward = orientation.getDirectionVector().normalized();
	Vector3 cameraRight = cameraForward.cross(Vector3::UP).normalized();
	Vector3 cameraUp = cameraRight.cross(cameraForward).normalized();
	Vector3 imagePlaneCenter = position + cameraForward * imagePlaneDistance;

	float rx = (float)x / (width - 1) - 0.5f;
	float ry = (float)y / (height - 1) - 0.5f;

	Vector3 imagePlanePosition = imagePlaneCenter + rx * cameraRight + ry * cameraUp;
	Vector3 rayDirection = (imagePlanePosition - position).normalized();

	return Ray(position, rayDirection);
}
