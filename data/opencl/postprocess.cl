kernel void postprocess(
	constant State* state,
	global float4* cumulativeColors,
	global float* filterWeights,
	write_only image2d_t outputImage)
{
	int x = get_global_id(0);
	int y = get_global_id(1);
	int index = y * state->imageWidth + x;

	float4 color = cumulativeColors[index] / filterWeights[index];
	color = clamp(pow(color, 1.0 / 2.2), 0.0, 1.0);
	color.w = 1.0;

	write_imagef(outputImage, (int2)(x, y), color);
}
