// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <vector>

#include "OpenCL/CLStructs.h"

namespace Raycer
{
	class Scene;
	class Vector3;
	class Color;

	class CLScene
	{
	public:

		void readSceneFull(const Scene& scene);
		void readSceneCamera(const Scene& scene);

		OpenCL::State state;
		OpenCL::Camera camera;
		OpenCL::Raytracer raytracer;
		OpenCL::ToneMapper toneMapper;
		OpenCL::SimpleFog simpleFog;
		std::vector<OpenCL::Material> materials;
		OpenCL::AmbientLight ambientLight;
		std::vector<OpenCL::DirectionalLight> directionalLights;
		std::vector<OpenCL::PointLight> pointLights;
		std::vector<OpenCL::Triangle> triangles;
		std::vector<OpenCL::BVHNode> bvhNodes;
	};
}
