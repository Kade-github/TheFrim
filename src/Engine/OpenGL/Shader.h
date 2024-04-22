/* Header guard */
#ifndef _SHADER_H_
#define _SHADER_H_

#include <unordered_map>
#include <string>

class Shader
{
    std::unordered_map<std::string, unsigned int> uniform_map;
public:
    unsigned int program = 0;

    ~Shader();

    void LoadShader(std::string vert_shader, std::string frag_shader);

    void CompileShader(std::string vert_src, std::string frag_src);

    void Bind();
    void Unbind();

    unsigned int GetUniformLocation(const std::string& name);

    void SetUniform1i(const std::string& name, int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniform2f(const std::string& name, float v0, float v1);
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniformMat4f(const std::string& name, const float* matrix);
    void SetUniformMat4f(const int loc_id, const float* matrix);
};

#endif