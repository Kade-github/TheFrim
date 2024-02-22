#include "Shader.h"
#include <fstream>
#include <iostream>
#include <glad/glad.h>

Shader::~Shader()
{
    glDeleteProgram(this->program);
}

void Shader::Bind()
{
    glUseProgram(program);


}

void Shader::Unbind()
{
    glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value)
{ 
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
    glUniform2f(glGetUniformLocation(program, name.c_str()), v0, v1);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    glUniform3f(glGetUniformLocation(program, name.c_str()), v0, v1, v2);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(glGetUniformLocation(program, name.c_str()), v0, v1, v2, v3);
}

void Shader::SetUniformMat4f(const std::string& name, const float* matrix)
{
    int active = glGetUniformLocation(program, name.c_str());

    glUniformMatrix4fv(active, 1, GL_FALSE, matrix);
}

void Shader::LoadShader(std::string vert_shader, std::string frag_shader)
{
    std::ifstream vertF(vert_shader);
    std::ifstream fragF(frag_shader);

    std::string vert;
    std::string frag;

    if (!vertF.is_open() || !fragF.is_open())
    {
        std::cout << "Couldn't find shaders." << std::endl;
        return;
    }

    std::string line;

    while (getline(vertF, line))
        vert += line + "\n";
    vertF.close();
    while (getline(fragF, line))
        frag += line + "\n";
    fragF.close();

    CompileShader(vert, frag);
}

void Shader::CompileShader(std::string vert_src, std::string frag_src)
{
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

    const char* vert_src_c = vert_src.c_str();
    const char* frag_src_c = frag_src.c_str();

    glShaderSource(vert, 1, &vert_src_c, NULL);
    glShaderSource(frag, 1, &frag_src_c, NULL);

    glCompileShader(vert);
    glCompileShader(frag);

    GLint success;
    GLchar infoLog[512];

    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vert, 512, NULL, infoLog);
        std::cout << "Failed compile shader: " << infoLog << std::endl;
    }

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        std::cout << "Failed compile shader: " << infoLog << std::endl;
    }

    program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "Failed linking shader: " << infoLog << std::endl;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}