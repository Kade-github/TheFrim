#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_uv;

out vec2 f_uv;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(v_position.xyz, 1.0);
	f_uv = v_uv;
}