#version 150

/*
Resampling fragment shader. Interpolates a bigger/smaller image from the original
using a sample of 4x4 pixels and an interpolation function.

Digital Image Processing: PIKS Inside
4.3.2. Interpolation Functions
13.5.1. Interpolation Methods
*/

uniform sampler2D texture0;
uniform float textureWidth;
uniform float textureHeight;
uniform float texelWidth;
uniform float texelHeight;

in Data
{
	vec2 texcoord;
} input;

out vec3 color;

#define INTERPOLATION_FUNCTION lanczos

float square(float x)
{
	x = abs(x) / 2.0f;
	return (x < 0.5f) ? 1.0f : 0.0f;
}

float triangle(float x)
{
	x = abs(x) / 2.0f;
	return (1.0f - x);
}

float bell(float x)
{
	x = abs(x);
	
	if (x < 0.5f)
		return 0.75f - (x * x);
	else if (x < 1.5f)
		return 0.5f * pow(x - 1.5f, 2.0f);
	else
		return 0.0f;
}

float bspline(float x)
{
	x = abs(x);
	
	if (x < 1.0f)
		return (2.0f / 3.0f) + 0.5f * (x * x * x) - (x * x);
	else
		return (1.0f / 6.0f) * pow(2.0f - x, 3.0f);
}

#define PI 3.1415926f
#define SINC(x) (sin(PI * (x)) / (PI * (x)))

float lanczos(float x)
{
	x = abs(x);
	
	if (x == 0.0f)
		return 1.0f;
		
	return SINC(x) * SINC(x / 2.0f);
}

void main()
{
	// coordinates on the sampled texture [0 .. width/height]
	float tx = input.texcoord.x * textureWidth;
	float ty = input.texcoord.y * textureHeight;
	
	// texel centered coordinates on the sampled texture [0 .. 1]
	float ctx = (floor(tx) + 0.5f) / textureWidth;
	float cty = (floor(ty) + 0.5f) / textureHeight;
	vec2 center = vec2(ctx, cty);
	
	// interpolation value within a pixel [0 .. 1]
	float ax = fract(tx);
	float ay = fract(ty);
	
	vec4 combinedColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 normalization = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	for(int x = -1; x <= 2; x++)
	{
		for(int y = -1; y <= 2; y++)
		{
			// texel offset
			float ofx = texelWidth * float(x);
			float ofy = texelHeight * float(y);
			vec2 offset = vec2(ofx, ofy);
			
			// evaluate the texel color
			vec4 c = texture(texture0, center + offset);
			
			// evaluate the interpolation function
			// argument will be in range [-2.0 .. 2.0]
			float fx = INTERPOLATION_FUNCTION(float(x) - ax);
			float fy = INTERPOLATION_FUNCTION((float(y) - ay));
			vec4 f = vec4(fx, fx, fx, fx) * vec4(fy, fy, fy, fy);
			
			combinedColor += c * f;
			normalization += f;
		}
	}
	
	color = (combinedColor / normalization).rgb;
}
