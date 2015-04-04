// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"
#include "Math/EulerAngle.h"

namespace Raycer
{
	class Ray;

	class Camera
	{
	public:

		Camera();

		void setFov(float fov);
		void setSize(int width, int height);
		
		Ray getRay(int x, int y) const;

		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
		EulerAngle orientation = EulerAngle(0.0f, 0.0f, 0.0f);

	private:

		float fov = 90.0f;
		float aspectRatio = 1.0f;
		float imagePlaneDistance = 1.0f;

		int width = 0;
		int height = 0;
	};
}
