/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


/* stb_image helpers */

unsigned char* loadTextureData(const char* path, int* width, int* height, int* channels)
{
	stbi_set_flip_vertically_on_load(true);
	return stbi_load(path, width, height, channels, 0);
}

void freeTextureData(unsigned char* data)
{
	stbi_image_free(data);
}

namespace Engine
{
    namespace Render
    {
        namespace OpenGL
        {
			Texture::Texture()
			{
				glGenVertexArrays(1, &this->vao);
				glGenBuffers(1, &this->vbo);
				glGenTextures(1, &textureID);

				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);
				glEnableVertexAttribArray(3);

			}

            Texture::~Texture()
            {
				glDeleteVertexArrays(1, &this->vao);
				glDeleteBuffers(1, &this->vbo);
                glDeleteTextures(1, &textureID);
                freeTextureData(textureData);
            }

            void Texture::BindTexture()
			{
				glBindTexture(GL_TEXTURE_2D, textureID);
			}

            void Texture::UnbindTexture()
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            }

			void Texture::LoadTexture(const char* path)
			{
                SetData(loadTextureData(path, &width, &height, &channels));
			}

			void Texture::SetData(unsigned char* data)
			{
				textureData = data;

				BindTexture();

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				if (channels == 3)
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
				else if (channels == 4)
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
				else
					Logging::Log("Loaded texture contains invalid channels.", "error");

				UnbindTexture();
			}

			void Texture::Render(std::list<Data::VertexData> vertexs)
			{
				BindTexture();

				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);

				int totalSize = 0;

				for (auto vertexGroup : vertexs)
				{
					glBufferData(GL_ARRAY_BUFFER, vertexGroup.vertexs.size() * sizeof(Data::Vertex2D), &vertexGroup.vertexs[0], GL_STATIC_DRAW);
					totalSize += vertexGroup.vertexs.size();
				}

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Data::Vertex2D), (void*)0);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Data::Vertex2D), (void*)offsetof(Data::Vertex2D, Data::Vertex2D::uvs));
				glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Data::Vertex2D), (void*)offsetof(Data::Vertex2D, Data::Vertex2D::color));

				glBindVertexArray(this->vao);
				glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
				glDrawElements(GL_TRIANGLES, totalSize, GL_UNSIGNED_INT, (void*)0);

				UnbindTexture();
			}
        }
    }
}