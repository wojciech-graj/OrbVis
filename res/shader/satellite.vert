#version 410 core
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;

layout (location = 0) uniform mat4 transform;

out vec3 color;

const float scaling_factor = 15.0;
const float max_size = 8.0;

void main()
{
	gl_Position = transform * vec4(in_pos, 1.0f);
	gl_PointSize = clamp(round(scaling_factor - scaling_factor * (gl_Position.z / gl_Position.w)), 1, max_size);
	color = in_color;
}
