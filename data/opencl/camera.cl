Ray getCameraRay(constant Camera* camera, int x, int y)
{
	float dx = ((float)x / camera->imagePlaneWidth) - 0.5;
	float dy = ((float)y / camera->imagePlaneHeight) - 0.5;

	float4 imagePlanePixelPosition = camera->imagePlaneCenter + (dx * camera->right) + (dy * camera->aspectRatio * camera->up);
	float4 direction = normalize(imagePlanePixelPosition - camera->position);

	return constructRay(camera->position, direction);
}
