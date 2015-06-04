#version 150

/*
Filtering fragment shader. Filtering is done with 2D convolution using different kernels.
Source and destination textures/rendertargets are of the same size.
*/

uniform sampler2D texture0;
uniform float textureWidth;
uniform float textureHeight;
uniform float texelWidth;
uniform float texelHeight;

in Data
{
	vec2 texcoord;
} fsin;

out vec3 color;

// uncomment this to bypass the filtering completely
#define BYPASS

/*
// SHARPEN //
const float kernel[9] = float[](
	0, -1, 0,
	-1, 5, -1,
	0, -1, 0
);
*/

/*
// BLUR //
const float kernel[9] = float[](
	0.0625f, 0.125f, 0.0625f,
	0.125f,  0.25f,  0.125f,
	0.0625f, 0.125f, 0.0625f
);
*/

/*
// EMBOSS //
const float kernel[9] = float[](
	-2, -1,  0,
	-1,  1,  1,
	 0,  1,  2
);
*/

// EDGE //
const float kernel[9] = float[](
	-1, -1, -1,
	-1,  8, -1,
	-1, -1, -1
);

void main()
{
#ifndef BYPASS
	vec4 combinedColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	for(int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			// texel offset
			float ofx = texelWidth * float(x);
			float ofy = texelHeight * float(y);
			vec2 offset = vec2(ofx, ofy);
			
			// evaluate the texel color
			vec4 c = texture(texture0, fsin.texcoord + offset);
			float f = kernel[(y + 1) * 3 + (x + 1)];
			
			combinedColor += c * f;
		}
	}
	
	color = combinedColor.rgb;
#else
	color = texture(texture0, fsin.texcoord).rgb;
#endif
}
