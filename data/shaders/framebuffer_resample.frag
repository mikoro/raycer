#version 330

/*
Resampling fragment shader. Interpolates a bigger/smaller image from the original
using a sample of 4x4 pixels and an interpolation function.

Digital Image Processing: PIKS Inside
4.3.2. Interpolation Functions
13.5.1. Interpolation Methods

Reconstruction Filters in Computer Graphics (Don P. Mitchell, Arun N. Netravali)
*/

in vec2 vTexcoord;
out vec4 fColor;

uniform sampler2D texture0;
uniform float textureWidth;
uniform float textureHeight;
uniform float texelWidth;
uniform float texelHeight;

#define INTERPOLATION_FUNCTION lanczos

float box(float x)
{
	x = abs(x) / 2.0f;
	return (x < 0.5f) ? 1.0f : 0.0f;
}

float tent(float x)
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

// cubic bspline
//#define B 1.0f
//#define C 0.0f

// Catmull-Rom spline
//#define B 0.0f
//#define C 0.5f

// good compromise
#define B (1.0f / 3.0f)
#define C (1.0f / 3.0f)

// suppresse postaliasing patterns
//#define B (3.0f / 2.0f)
//#define C (-1.0f / 4.0f)

float mitchell(float x)
{
	x = abs(x);
	
	if (x < 1.0f)
		return ((12.0f - 9.0f * B - 6.0f * C) * (x * x * x) + (-18.0f + 12.0f * B + 6.0f * C) * (x * x) + (6.0f - 2.0f * B)) * (1.0f / 6.0f);
	else
		return ((-B - 6.0f * C) * (x * x * x) + (6.0f * B + 30.0f * C) * (x * x) + (-12.0f * B - 48.0f * C) * x + (8.0f * B + 24.0f * C)) * (1.0f / 6.0f);
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
	float tx = vTexcoord.x * textureWidth;
	float ty = vTexcoord.y * textureHeight;
	
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
	
	fColor = (combinedColor / normalization);
}
