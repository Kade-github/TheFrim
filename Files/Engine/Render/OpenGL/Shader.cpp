#include "Shader.h"
/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

namespace Engine
{
    namespace Render
    {
        namespace OpenGL
        {
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

            void Shader::LoadShader(std::string vert_shader, std::string frag_shader)
            {
                std::ifstream vertF(vert_shader);
                std::ifstream fragF(frag_shader);

                std::string vert;
                std::string frag;

                if (!vertF.is_open() || !fragF.is_open())
                {
                    Logging::Log(vert_shader + " or " + frag_shader + " cannot be found!", "error");
                    return;
                }

                std::string line;

                while (getline(vertF, line))
                    vert += line;
                vertF.close();
                while (getline(fragF, line))
                    frag += line;
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
					Logging::Log("Vertex shader compilation failed: " + std::string(infoLog), "error");
				}

				glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(frag, 512, NULL, infoLog);
					Logging::Log("Fragment shader compilation failed: " + std::string(infoLog), "error");
				}

				program = glCreateProgram();
				glAttachShader(program, vert);
				glAttachShader(program, frag);
				glLinkProgram(program);

				glGetProgramiv(program, GL_LINK_STATUS, &success);
				if (!success)
				{
					glGetProgramInfoLog(program, 512, NULL, infoLog);
					Logging::Log("Shader program linking failed: " + std::string(infoLog), "error");
				}

				glDeleteShader(vert);
				glDeleteShader(frag);

                Logging::Log("Shader program compiled successfully!", "info");

            }

        }
    }
}