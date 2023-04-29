#version 410 core

out vec4 frag_color;

in vec2 uv;

uniform sampler2D texture_sky;

void main()
{
	frag_color = texture(texture_sky, uv);
}
