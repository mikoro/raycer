// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"
#include "Math/EulerAngle.h"

namespace Raycer
{
	class App;
	struct Settings;
	struct Ray;

	class Camera
	{
	public:

		void initialize(App* app, Settings* settings);
		void setFov(double fov);
		void setImagePlaneSize(int width, int height);
		void update(double timeStep);
		
		Ray getRay(int x, int y) const;

		Vector3 position = Vector3(0.0, 0.0, 0.0);
		EulerAngle orientation = EulerAngle(0.0, 0.0, 0.0);

	private:

		App* app;
		Settings* settings;

		Vector3 forward;
		Vector3 right;
		Vector3 up;
		Vector3 imagePlaneCenter;

		double aspectRatio = 1.0;
		double imagePlaneDistance = 1.0;
		double imagePlaneWidth = 0.0;
		double imagePlaneHeight = 0.0;
	};
}
