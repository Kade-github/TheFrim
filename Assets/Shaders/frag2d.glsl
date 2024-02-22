#version 330 core
out vec4 FragColor;

uniform sampler2D u_texture;
in vec2 f_uv;

void main()
{
	FragColor =	texture2D(u_texture, f_uv);
}