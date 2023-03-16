#version 410 core
#extension GL_ARB_explicit_uniform_location : enable

out vec4 frag_color;

in vec2 uv;

layout (location = 0) uniform sampler2D texture_sky;

void main()
{
	frag_color = texture(texture_sky, uv);
}
