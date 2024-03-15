#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform sampler2D ourTexture;

void main()
{
	vec3 lightColor = vec3(1.0,1.0,1.0);

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

	FragColor = texture2D(ourTexture, TexCoord) * vec4(ambient + diffuse, 1.0);
}