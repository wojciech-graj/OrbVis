#version 410 core
#extension GL_ARB_explicit_uniform_location : enable

out vec4 frag_color;

in vec3 color;

void main()
{
	frag_color = vec4(color, 1.0f);
}
