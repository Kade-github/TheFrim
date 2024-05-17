#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Position;
in vec3 Normal;

uniform sampler2D ourTexture;

uniform int useAmbientDiffusion;

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
    if (useAmbientDiffusion == 1)
    {
        vec3 x = dFdx(Position);
        vec3 y = dFdy(Position);
        vec3 normal = cross(x, y);
        vec3 norm = normalize(normal);

        vec3 diffColor1 = vec3(1.0, 1.0, 1.0);

        vec3 lightDir1 = normalize(CameraPos - vec3(Position.x, Position.y, Position.z)) * 1.2;
        float diff1 = max(dot(norm, lightDir1), 0.0);
        vec3 diffuse = diff1 * diffColor1;

        color = color * vec4(diffuse, 1.0);
    }

    // darken based on the light level

    float light = max(0.2, lightLevel / 10.0);

	FragColor = mix(color, vec4(0.0, 0.0, 0.0, color.a), 1.0 - light);

    // Mix in redness

    FragColor = mix(FragColor, vec4(1.0, 0.0, 0.0, FragColor.a), redness);

    if (FogFar < 1000)
	    FragColor = mix(FragColor, vec4(FogColor, 1.0), fogFactor);

}