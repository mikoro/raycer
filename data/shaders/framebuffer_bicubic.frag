#version 330

uniform sampler2D tex0;
uniform float textureWidth;
uniform float textureHeight;
uniform float texelWidth;
uniform float texelHeight;

in vec2 tex_coord;

out vec3 color;

// select one
// triangle, bell, bspline, catmullrom, lanczos
#define INTERPOLATION_FUNCTION triangle

// can be tuned (1.0f - 3.0f)
#define LANCZOS_SIZE 2.0f

// don't touch
#define PI 3.14159265f
#define SINC(x) (sin(PI * (x)) / (PI * (x)))
#define X_RANGE 2.0f

float triangle(float x)
{
	x = x / X_RANGE;
	
	if(x <= 0.0f)
		return (x + 1.0f);
	else
		return (1.0f - x);
}

float bell(float x)
{
	x = (x / X_RANGE) * 1.5f;
	
	if(x >= -1.5f && x <= -0.5f)
	{
		return(0.5f * pow(x + 1.5f, 2.0f));
	}
	else if(x > -0.5f && x <= 0.5f)
	{
		return 3.0f / 4.0f - (x * x);
	}
	else if(x > 0.5f && x <= 1.5f)
	{
		return(0.5f * pow(x - 1.5f, 2.0f));
	}
	else
		return 0.0f;
}

float bspline(float x)
{
	x = (abs(x) / X_RANGE) * 2.0f;

	if(x >= 0.0f && x <= 1.0f)
	{
		return (2.0f / 3.0f) + 0.5f * (x * x * x) - (x * x);
	}
	else if(x > 1.0f && x <= 2.0f)
	{
		return (1.0f / 6.0f) * pow(2.0f - x, 3.0f);
	}
	else
		return 0.0f;
}  

float catmullrom(float x)
{
    const float B = 0.0f;
    const float C = 0.5f;
    
    x = (abs(x) / X_RANGE) * 2.0f;
    
	if(x < 1.0f)
	{
		return ((12 - 9 * B - 6 * C) * (x * x * x) +
		(-18 + 12 * B + 6 * C) * (x * x) +
		(6 - 2 * B)) / 6.0f;
	}
	else if(x >= 1.0f && x <= 2.0f)
	{
		return ((-B - 6 * C) * (x * x * x) +
		(6 * B + 30 * C) * (x * x) +
		(-12 * B - 48 * C) * x +
		8 * B + 24 * C) / 6.0f;
	}
	else
		return 0.0f;
}

float lanczos(float x)
{
	x = (abs(x) / X_RANGE) * LANCZOS_SIZE;

	if (x == 0.0f)
		return 1.0f;
	else
		return SINC(x) * SINC(x / LANCZOS_SIZE);
}

void main()
{
	// round up to the nearest texel center (this avoids hardware bilinear)
	float tx = tex_coord.x * textureWidth;
	tx = ceil(tx + 0.5f) - 0.5f;

	float ty = tex_coord.y * textureHeight;
	ty = ceil(ty + 0.5f) - 0.5f;

	vec2 snappedTextureCoordinate = vec2(tx / textureWidth, ty / textureHeight);
	
	float alphaX = fract(tex_coord.x * textureWidth);
	float alphaY = fract(tex_coord.y * textureHeight);
	
	// remap alpha 0.5 1.0 0.5 -> 0.0 0.5 1.0
	// the snapping makes this necessary
	alphaX += (alphaX > 0.5f) ? -0.5f : 0.5f;
	alphaY += (alphaY > 0.5f) ? -0.5f : 0.5f;
	
	vec4 num = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 den = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	for(int x = -1; x <= 2; x++)
	{
		for(int y = -1; y <= 2; y++)
		{
			vec4 color = texture(tex0, snappedTextureCoordinate + vec2(texelWidth * float(x), texelHeight * float(y)));
				
			float f1 = INTERPOLATION_FUNCTION(float(x) - alphaX); // argument range is -2.0f - 2.0f
			float f2 = INTERPOLATION_FUNCTION((float(y) - alphaY));  // argument range is -2.0f - 2.0f
			vec4 f1vec = vec4(f1, f1, f1, f1);
			vec4 f2vec = vec4(f2, f2, f2, f2);
			vec4 combined = f1vec * f2vec;
			
			num += color * combined;
			den += combined;
		}
	}
	
	color = (num / den).rgb;
}
