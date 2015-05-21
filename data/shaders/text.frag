#version 150

uniform sampler2D tex0;

in vec2 tex_coord_var;
in vec4 color_var;

out vec4 color_final;

void main()
{
	float a = texture(tex0, tex_coord_var).r;
	color_final = vec4(color_var.rgb, color_var.a * a);
}
