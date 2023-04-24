#version 410 core
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;

layout (location = 0) uniform mat4 transform;
layout (location = 1) uniform float scl;

out vec3 color;

const float scaling_factor = 15.0f;
const float max_size = 6.0f;

void main()
{
	gl_Position = transform * vec4(in_pos, 1.0f);
	gl_PointSize = clamp(round(scaling_factor * scl * (1.0f - (gl_Position.z / gl_Position.w))), scl, max_size * (scl + 1.0f));
	color = in_color;
}
