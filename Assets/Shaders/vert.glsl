#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Position = vec3(model * vec4(aPos, 1.0));
    TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}