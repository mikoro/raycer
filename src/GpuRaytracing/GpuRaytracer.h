// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class OpenCLHelper;
	class Scene;
	class Image;

	class GpuRaytracer
	{
	public:

		GpuRaytracer(OpenCLHelper& helper);

		void setSize(int width, int height);
		void trace(const Scene& scene);
		Image& getImage();

	private:

		OpenCLHelper& helper;
	};
}
