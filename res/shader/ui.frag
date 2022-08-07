#version 460 core

out vec4 frag_color;

in vec2 uv;

layout (location = 0) uniform sampler2D texture_atlas;
layout (location = 1) uniform vec4 color;

void main()
{
	frag_color = texture(texture_atlas, uv).rrrr * color;
}
