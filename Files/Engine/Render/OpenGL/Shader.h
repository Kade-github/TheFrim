/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _SHADER_H_
#define _SHADER_H_

#include "../../../../Includes.h"

#include "../../Logging.h"

namespace Engine
{
    namespace Render
    {
        namespace OpenGL
        {
            class Shader 
            {
            private:
                GLuint program = 0;

                std::unordered_map<std::string, GLint> uniform_map;

				GLint GetUniformLocation(std::string loc)
				{
					//Use previously defined location
					auto key_loc = uniform_map.find(loc);
					if (key_loc != uniform_map.end())
						return key_loc->second;

					//Find and remember location
					GLint uniform_loc = glGetUniformLocation(program, loc.c_str());
					if (uniform_loc == -1)
					{
						return -1;
					}
					uniform_map[loc] = uniform_loc;
					return uniform_loc;
				}

				#define SETUNIFORM_GET_LOC_ID() \
					GLint loc_id = GetUniformLocation(loc); \
						if (loc_id == -1) \
							return true;
            public:

                ~Shader();

                void LoadShader(std::string vert_shader, std::string frag_shader);

				void CompileShader(std::string vert_src, std::string frag_src);

                void Bind();
                void Unbind();


				bool SetUniform(std::string loc, int i0)
				{
					SETUNIFORM_GET_LOC_ID()
						glUniform1i(loc_id, i0);
					return false;
				}

				bool SetUniform(std::string loc, int i0, int i1)
				{
					SETUNIFORM_GET_LOC_ID()
						glUniform2i(loc_id, i0, i1);
					return false;
				}

				bool SetUniform(std::string loc, int i0, int i1, int i2)
				{
					SETUNIFORM_GET_LOC_ID()
						glUniform3i(loc_id, i0, i1, i2);
					return false;
				}

				bool SetUniform(std::string loc, int i0, int i1, int i2, int i3)
				{
					SETUNIFORM_GET_LOC_ID()
						glUniform4i(loc_id, i0, i1, i2, i3);
					return false;
				}

				bool SetUniform(std::string loc, float f0)
				{
					SETUNIFORM_GET_LOC_ID()
						glUniform1f(loc_id, f0);
					return false;
				}

				bool SetUniform(std::string loc, float f0, float f1)
				{
					SETUNIFORM_GET_LOC_ID()
						glUniform2f(loc_id, f0, f1);
					return false;
				}

				bool SetUniform(std::string loc, float f0, float f1, float f2)
				{
					SETUNIFORM_GET_LOC_ID()
						glUniform3f(loc_id, f0, f1, f2);
					return false;
				}

				bool SetUniform(std::string loc, float f0, float f1, float f2, float f3)
				{
					SETUNIFORM_GET_LOC_ID()
						glUniform4f(loc_id, f0, f1, f2, f3);
					return false;
				}

				bool SetUniform(std::string loc, int count, float* ptr)
				{
					SETUNIFORM_GET_LOC_ID()
						glUniformMatrix4fv(loc_id, count, GL_FALSE, ptr);
					return false;
				}

            };
        }
    }
}


#endif