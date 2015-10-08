kernel void printSizes()
{
	printf("\nState: %d\n", sizeof(State));
	printf("General: %d\n", sizeof(General));
	printf("Camera: %d\n", sizeof(Camera));
	printf("ToneMapper: %d\n", sizeof(ToneMapper));
	printf("SimpleFog: %d\n", sizeof(SimpleFog));
	printf("Material: %d\n", sizeof(Material));
	printf("AmbientLight: %d\n", sizeof(AmbientLight));
	printf("DirectionalLight: %d\n", sizeof(DirectionalLight));
	printf("PointLight: %d\n", sizeof(PointLight));
	printf("Triangle: %d\n", sizeof(Triangle));
	printf("AABB: %d\n", sizeof(AABB));
	printf("BVHNode: %d\n", sizeof(BVHNode));
}
