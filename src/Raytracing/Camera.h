// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"
#include "Math/EulerAngle.h"

namespace Raycer
{
	class App;
	struct Settings;
	class Ray;

	class Camera
	{
	public:

		void initialize(App* app, Settings* settings);
		void setFov(float fov);
		void setImagePlaneSize(int width, int height);
		void update(float timeStep);
		
		Ray getRay(int x, int y) const;

		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
		EulerAngle orientation = EulerAngle(0.0f, 0.0f, 0.0f);

	private:

		App* app;
		Settings* settings;

		Vector3 forward;
		Vector3 right;
		Vector3 up;
		Vector3 imagePlaneCenter;

		float aspectRatio = 1.0f;
		float imagePlaneDistance = 1.0f;
		float imagePlaneWidth = 0.0f;
		float imagePlaneHeight = 0.0f;
	};
}
