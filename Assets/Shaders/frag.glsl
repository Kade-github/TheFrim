#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Position;

uniform sampler2D ourTexture;

uniform vec3 CameraPos;
uniform float FogFar;
uniform vec3 FogColor;
uniform float lightLevel;
uniform float redness;
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
    {
        // darken based on the light level

        float light = max(0.5, lightLevel / 10.0);

	    FragColor = mix(color, vec4(0.0, 0.0, 0.0, color.a), 1.0 - light);

        // Mix in redness

        FragColor = mix(FragColor, vec4(1.0, 0.0, 0.0, FragColor.a), redness);

	    FragColor = mix(FragColor, vec4(FogColor, 1.0), fogFactor);
    }
    else
		FragColor = color;
}