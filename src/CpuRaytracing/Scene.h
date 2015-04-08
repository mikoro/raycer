// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>
#include <memory>

#include "CpuRaytracing/Primitive.h"
#include "CpuRaytracing/Sphere.h"
#include "CpuRaytracing/Plane.h"
#include "CpuRaytracing/Mesh.h"
#include "CpuRaytracing/Light.h"
#include "CpuRaytracing/Camera.h"
#include "Math/Color.h"

namespace Raycer
{
	class BaseLog;
	class NamedLog;

	class Scene
	{
	public:

		Scene(BaseLog& baseLog);

		void load(const std::string fileName);
		void saveAs(const std::string fileName) const;

		void initialize();

		std::vector<Primitive*> primitives;

		std::vector<Sphere> spheres;
		std::vector<Plane> planes;
		std::vector<Mesh> meshes;
		std::vector<Light> lights;

		Camera camera;

		int maxReflections = 0;

		bool fogEnabled = false;
		double fogDistance = 0.0;
		double fogSteepness = 0.0;
		Color fogColor = Color(0.0, 0.0, 0.0);

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(spheres),
				CEREAL_NVP(planes),
				CEREAL_NVP(meshes),
				CEREAL_NVP(lights),
				CEREAL_NVP(camera),
				CEREAL_NVP(maxReflections),
				CEREAL_NVP(fogEnabled),
				CEREAL_NVP(fogDistance),
				CEREAL_NVP(fogSteepness),
				CEREAL_NVP(fogColor));
		}

	private:

		std::unique_ptr<NamedLog> log;
	};
}
