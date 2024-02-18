#version 330 core
out vec4 FragColor;
  
uniform vec3 lightPos;
uniform vec3 lightColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}