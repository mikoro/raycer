#version 150

uniform sampler2D texture0;

in Data
{
	vec2 texcoord;
} input;

out vec3 color;

void main()
{
	color = texture(texture0, input.texcoord).rgb;
}
