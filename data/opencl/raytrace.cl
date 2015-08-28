kernel void raytrace(write_only image2d_t image,
	constant State* state,
	constant Camera* camera,
	constant Raytracer* raytracer,
	constant ToneMapper* toneMapper,
	constant SimpleFog* simpleFog,
	constant Material* materials,
	constant AmbientLight* ambientLight,
	constant DirectionalLight* directionalLights,
	constant PointLight* pointLights,
	constant SpotLight* spotLights,
	constant Plane* planes,
	constant Sphere* spheres,
	constant Box* boxes,
	constant Triangle* triangles)
{
	int x = get_global_id(0);
	int y = get_global_id(1);

	Ray ray = getCameraRay(camera, x, y);
	Intersection intersection = constructIntersection();
	float4 finalColor = raytracer->backgroundColor;

	for (int i = 0; i < state->planeCount; ++i)
		intersectPlane(&planes[i], &ray, &intersection, materials);

	if (intersection.wasFound)
	{
		constant Material* material = &materials[intersection.materialIndex];
		finalColor = material->color;
	}

	write_imagef(image, (int2)(x, y), finalColor);
}
