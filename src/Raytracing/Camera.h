// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"
#include "Math/EulerAngle.h"

namespace Raycer
{
	class InteractiveRunner;
	struct InteractiveSettings;
	struct Ray;

	class Camera
	{
	public:

		void initialize(InteractiveRunner* runner, InteractiveSettings* settings);
		void setImagePlaneSize(int width, int height);
		void calculateVariables();
		void update(double timeStep);
		
		Ray getRay(int x, int y) const;

		Vector3 position = Vector3(0.0, 0.0, 0.0);
		EulerAngle orientation = EulerAngle(0.0, 0.0, 0.0);
		double fov = 75.0;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(position),
				CEREAL_NVP(orientation),
				CEREAL_NVP(fov));
		}

	private:

		InteractiveRunner* runner = nullptr;
		InteractiveSettings* settings = nullptr;

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
