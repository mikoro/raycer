// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"
#include "Math/EulerAngle.h"

namespace Raycer
{
	class Ray;
	class Vector2;
	class Scene;
	class Primitive;

	enum class CameraProjectionType { PERSPECTIVE, ORTHOGRAPHIC, FISHEYE };

	class Camera
	{
	public:

		Camera();

		void initialize();
		void setImagePlaneSize(int width, int height);
		void update(const Scene& scene, double timeStep);
		void precalculate();

		Ray getRay(const Vector2& pixelCoordinate) const;

		Vector3 position;
		EulerAngle orientation;
		CameraProjectionType projectionType = CameraProjectionType::PERSPECTIVE;
		double fov = 75.0;
		double orthoSize = 10.0;
		double fishEyeAngle = 180.0;

		int dofSamples = 0;
		int timeSamples = 0;
		double apertureSize = 0.1;
		double focalLenght = 10.0;

		Vector3 forward;
		Vector3 right;
		Vector3 up;
		Vector3 imagePlaneCenter;

		double aspectRatio = 1.0;
		double imagePlaneDistance = 1.0;
		double imagePlaneWidth = 0.0;
		double imagePlaneHeight = 0.0;

	private:

		Vector3 velocity;
		Vector3 acceleration;
		Vector3 angularVelocity;
		Vector3 angularAcceleration;

		bool isMovingPrimitive = false;
		Primitive* movingPrimitive = nullptr;
	};
}
