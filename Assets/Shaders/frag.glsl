#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Position;

uniform sampler2D ourTexture;

uniform vec3 CameraPos;
uniform float FogFar;
uniform vec3 FogColor;
float FogMin = 0.0;

float getFogFactor(float d)
{

    if (d>=FogFar) return 1.0;
    if (d<=FogMin) return 0.0;

    return 1 - (FogFar - d) / (FogFar - FogMin);
}

void main()
{
    FogMin = FogFar / 8.0;

	vec4 color = texture(ourTexture, TexCoord);

    float d = distance(CameraPos, Position);
	float fogFactor = getFogFactor(d);

    if (FogFar < 1000)
	    FragColor = mix(color, vec4(FogColor, 1.0), fogFactor);
    else
		FragColor = color;
}