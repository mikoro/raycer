#version 330

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexcoord;

out vec2 vTexcoord;

void main()
{
	vTexcoord = aTexcoord;
	gl_Position = vec4(aPosition, 0.0f, 1.0f);
}
