#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_uv;
layout (location = 2) in vec4 v_color;

out vec2 f_uv;
out vec4 f_color;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(v_position.xyz, 1.0);
	f_color = v_color;
	f_uv = v_uv;
}