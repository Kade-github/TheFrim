#version 330 core

in vec2 v_position;
in vec2 v_uv;
out vec2 f_uv;
out vec4 f_colour;
uniform mat4 u_projection;

void main()
{
	f_uv = v_uv;
	gl_Position = u_projection * vec4(v_position.xy, 0.0, 1.0);
}