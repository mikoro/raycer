// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Math/Vector3.h"
#include "Math/EulerAngle.h"

namespace Raycer
{
	struct Ray;

	class Camera
	{
	public:

		void initialize();
		void setImagePlaneSize(int width, int height);
		void update(double timeStep);
		void precalculate();

		Ray getRay(double x, double y) const;

		Vector3 position;
		EulerAngle orientation;
		double fov = 75.0;

		Vector3 forward;
		Vector3 right;
		Vector3 up;
		Vector3 imagePlaneCenter;

		double aspectRatio = 1.0;
		double imagePlaneDistance = 1.0;
		double imagePlaneWidth = 0.0;
		double imagePlaneHeight = 0.0;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(position),
				CEREAL_NVP(orientation),
				CEREAL_NVP(fov));
		}

	private:

		Vector3 velocity;
		Vector3 acceleration;
		Vector3 angularVelocity;
		Vector3 angularAcceleration;
	};
}
