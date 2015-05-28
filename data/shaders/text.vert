#version 150

uniform mat4 mvp;

in vec3 vertex;
in vec2 tex_coord;
in vec4 color;

out Data
{
	vec2 texcoord;
	vec4 color;
} output;

void main()
{
	gl_Position = mvp * vec4(vertex, 1.0f);
	output.texcoord = tex_coord;
	output.color = color;
}
