#version 150

uniform mat4 mvp;

in vec3 vertex;
in vec2 tex_coord;
in vec4 color;

out vec2 tex_coord_var;
out vec4 color_var;

void main()
{
	gl_Position = mvp * vec4(vertex, 1.0f);
	tex_coord_var = tex_coord;
	color_var = color;
}
