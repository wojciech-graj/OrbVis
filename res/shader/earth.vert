#version 410 core
#extension GL_ARB_explicit_uniform_location : enable
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_uv;

out vec3 pos;
out vec2 uv;

layout (location = 5) uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(in_pos, 1.0);
	uv = in_uv;
	pos = in_pos;
}
