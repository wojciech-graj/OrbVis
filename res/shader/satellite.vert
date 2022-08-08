#version 430 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;

layout (location = 0) uniform mat4 transform;

out vec3 color;

const float scaling_factor = 0.05;
const float max_size = 10.0;

void main()
{
	gl_Position = transform * vec4(in_pos, 1.0f);
	gl_PointSize = min(round((1.0 + scaling_factor) / (scaling_factor + gl_Position.z / gl_Position.w)), max_size);
	color = in_color;
}
