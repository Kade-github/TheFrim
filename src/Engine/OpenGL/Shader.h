/* Header guard */
#ifndef _SHADER_H_
#define _SHADER_H_

#include "../../../Include/glad/glad.h"
#include <unordered_map>
#include <string>

class Shader
{
public:
    GLuint program = 0;

    ~Shader();

    void LoadShader(std::string vert_shader, std::string frag_shader);

    void CompileShader(std::string vert_src, std::string frag_src);

    void Bind();
    void Unbind();
};

#endif