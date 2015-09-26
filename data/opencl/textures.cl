float4 getTextureColor(int textureIndex, float2 texcoord, TEX_INPUT_ARGS)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
	float4 color = (float4)(1.0, 0.0, 0.0, 1.0);

	switch(textureIndex)
	{
		case 0: color = read_imagef(tex0, sampler, texcoord); break;
		case 1: color = read_imagef(tex1, sampler, texcoord); break;
		case 2: color = read_imagef(tex2, sampler, texcoord); break;
		case 3: color = read_imagef(tex3, sampler, texcoord); break;
		case 4: color = read_imagef(tex4, sampler, texcoord); break;
		case 5: color = read_imagef(tex5, sampler, texcoord); break;
		case 6: color = read_imagef(tex6, sampler, texcoord); break;
		case 7: color = read_imagef(tex7, sampler, texcoord); break;
		case 8: color = read_imagef(tex8, sampler, texcoord); break;
		case 9: color = read_imagef(tex9, sampler, texcoord); break;
		default: break;
	}

	return color;
}
