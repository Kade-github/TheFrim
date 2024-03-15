#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;

uniform sampler2D ourTexture;

void main()
{
	FragColor = texture2D(ourTexture, TexCoord);
}