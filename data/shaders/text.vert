#version 150

uniform mat4 mvp;

in vec3 vertex;
in vec2 tex_coord;
in vec4 color;

out Data
{
	vec2 texcoord;
	vec4 color;
} vsout;

void main()
{
	gl_Position = mvp * vec4(vertex, 1.0f);
	vsout.texcoord = tex_coord;
	vsout.color = color;
}
