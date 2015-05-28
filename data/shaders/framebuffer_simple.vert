#version 150

in vec2 vertex;
in vec2 texcoord;

out Data
{
	vec2 texcoord;
} output;

void main()
{
	gl_Position = vec4(vertex, 1.0f, 1.0f);
	output.texcoord = texcoord;
}
