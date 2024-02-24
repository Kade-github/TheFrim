#version 330 core
out vec4 FragColor;

uniform sampler2D u_texture;
in vec2 f_uv;
in vec4 f_color;

void main()
{
	FragColor =	texture(u_texture, f_uv) * f_color;
}