kernel void printSizes()
{
	printf("\nState: %d\n", sizeof(State));
	printf("Camera: %d\n", sizeof(Camera));
	printf("Raytracer: %d\n", sizeof(Raytracer));
	printf("ToneMapper: %d\n", sizeof(ToneMapper));
	printf("SimpleFog: %d\n", sizeof(SimpleFog));
	printf("Material: %d\n", sizeof(Material));
	printf("AmbientLight: %d\n", sizeof(AmbientLight));
	printf("DirectionalLight: %d\n", sizeof(DirectionalLight));
	printf("PointLight: %d\n", sizeof(PointLight));
	printf("SpotLight: %d\n", sizeof(SpotLight));
	printf("Plane: %d\n", sizeof(Plane));
	printf("Sphere: %d\n", sizeof(Sphere));
	printf("Box: %d\n", sizeof(Box));
	printf("Triangle: %d\n", sizeof(Triangle));
}
