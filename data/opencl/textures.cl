float4 getTextureColor(int textureIndex, float2 texcoord, TEX_INPUT_ARGS)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
	float4 color = (float4)(0.0, 0.0, 0.0, 1.0);

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
		case 10: color = read_imagef(tex10, sampler, texcoord); break;
		case 11: color = read_imagef(tex11, sampler, texcoord); break;
		case 12: color = read_imagef(tex12, sampler, texcoord); break;
		case 13: color = read_imagef(tex13, sampler, texcoord); break;
		case 14: color = read_imagef(tex14, sampler, texcoord); break;
		case 15: color = read_imagef(tex15, sampler, texcoord); break;
		case 16: color = read_imagef(tex16, sampler, texcoord); break;
		case 17: color = read_imagef(tex17, sampler, texcoord); break;
		case 18: color = read_imagef(tex18, sampler, texcoord); break;
		case 19: color = read_imagef(tex19, sampler, texcoord); break;
		case 20: color = read_imagef(tex20, sampler, texcoord); break;
		case 21: color = read_imagef(tex21, sampler, texcoord); break;
		case 22: color = read_imagef(tex22, sampler, texcoord); break;
		case 23: color = read_imagef(tex23, sampler, texcoord); break;
		case 24: color = read_imagef(tex24, sampler, texcoord); break;
		case 25: color = read_imagef(tex25, sampler, texcoord); break;
		case 26: color = read_imagef(tex26, sampler, texcoord); break;
		case 27: color = read_imagef(tex27, sampler, texcoord); break;
		case 28: color = read_imagef(tex28, sampler, texcoord); break;
		case 29: color = read_imagef(tex29, sampler, texcoord); break;
		case 30: color = read_imagef(tex30, sampler, texcoord); break;
		case 31: color = read_imagef(tex31, sampler, texcoord); break;
		case 32: color = read_imagef(tex32, sampler, texcoord); break;
		case 33: color = read_imagef(tex33, sampler, texcoord); break;
		case 34: color = read_imagef(tex34, sampler, texcoord); break;
		case 35: color = read_imagef(tex35, sampler, texcoord); break;
		case 36: color = read_imagef(tex36, sampler, texcoord); break;
		case 37: color = read_imagef(tex37, sampler, texcoord); break;
		case 38: color = read_imagef(tex38, sampler, texcoord); break;
		case 39: color = read_imagef(tex39, sampler, texcoord); break;
		case 40: color = read_imagef(tex40, sampler, texcoord); break;
		case 41: color = read_imagef(tex41, sampler, texcoord); break;
		case 42: color = read_imagef(tex42, sampler, texcoord); break;
		case 43: color = read_imagef(tex43, sampler, texcoord); break;
		case 44: color = read_imagef(tex44, sampler, texcoord); break;
		case 45: color = read_imagef(tex45, sampler, texcoord); break;
		case 46: color = read_imagef(tex46, sampler, texcoord); break;
		case 47: color = read_imagef(tex47, sampler, texcoord); break;
		case 48: color = read_imagef(tex48, sampler, texcoord); break;
		case 49: color = read_imagef(tex49, sampler, texcoord); break;
		case 50: color = read_imagef(tex50, sampler, texcoord); break;
		case 51: color = read_imagef(tex51, sampler, texcoord); break;
		case 52: color = read_imagef(tex52, sampler, texcoord); break;
		case 53: color = read_imagef(tex53, sampler, texcoord); break;
		case 54: color = read_imagef(tex54, sampler, texcoord); break;
		case 55: color = read_imagef(tex55, sampler, texcoord); break;
		case 56: color = read_imagef(tex56, sampler, texcoord); break;
		case 57: color = read_imagef(tex57, sampler, texcoord); break;
		case 58: color = read_imagef(tex58, sampler, texcoord); break;
		case 59: color = read_imagef(tex59, sampler, texcoord); break;
		default: break;
	}

	return color;
}
