#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Position;

uniform sampler2D ourTexture;

uniform vec3 CameraPos;
uniform float FogFar;
uniform vec3 FogColor;

float getFogFactor(float d)
{
    const float FogMin = 0;

    if (d>=FogFar) return 1;
    if (d<=FogMin) return 0;

    return 1 - (FogFar - d) / (FogFar - FogMin);
}

void main()
{
	vec4 color = texture(ourTexture, TexCoord);

    float d = distance(CameraPos, Position);
	float fogFactor = getFogFactor(d);

	FragColor = mix(color, vec4(FogColor, 1.0), fogFactor);
}