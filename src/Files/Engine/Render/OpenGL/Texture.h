/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../../../../Includes.h"
#include "../../Data/VertexData.h"

#include "../../Logging.h"

namespace Engine
{
    namespace Render
    {
        namespace OpenGL
        {
            class Texture
            {
            private:
                unsigned int vao, vbo;
                GLuint textureID = 0;
                unsigned char* textureData;

                int channels;

            public:

                int width;
                int height;

                Texture();
				~Texture();

				/// <summary>
				/// Load a texture from a file, then set the Texture's data to the loaded texture.
				/// </summary>
				/// <param name="path">The file path relative to the executable.</param>
				void LoadTexture(const char* path);

                /// <summary>
                /// Set the textures data.
                /// </summary>
                /// <param name="data">The data in char* form</param>
                void SetData(unsigned char* data);

				/// <summary>
				/// Bind the texture to the current OpenGL context.
				/// </summary>
				void BindTexture();

				/// <summary>
				/// Unbind the texture from the current OpenGL context.
				/// </summary>
				void UnbindTexture();

                /// <summary>
                /// Draw 2D vertices to the screen of this texture.
                /// </summary>
                /// <param name="vertexs">Every vertex of this texture used</param>
                void Render(std::list<Data::VertexData> vertexs);
            };
        }
    }
}


#endif