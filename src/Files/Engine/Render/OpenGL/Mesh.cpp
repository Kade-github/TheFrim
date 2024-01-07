/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

#include "Mesh.h"

namespace Engine
{
    namespace Render
    {
        namespace OpenGL
        {
            Mesh::Mesh(Data::MeshData m)
            {
			    this->meshData = m;

                Create();
            }

            Mesh::~Mesh()
			{
				glDeleteVertexArrays(1, &this->vao);
				glDeleteBuffers(1, &this->vbo);
                glDeleteBuffers(1, &this->ebo);
			}

            void Mesh::Create()
            {
                glGenVertexArrays(1, &this->vao);
                glGenBuffers(1, &this->vbo);
                glGenBuffers(1, &this->ebo);

                glBindVertexArray(this->vao);
                glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

                glBufferData(GL_ARRAY_BUFFER, this->meshData.vertices.size() * sizeof(Data::Vertex), &this->meshData.vertices[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->meshData.indices.size() * sizeof(unsigned int), &this->meshData.indices[0], GL_STATIC_DRAW);

                // vertex positions
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Data::Vertex), (void*)0);
                // vertex texture coords
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Data::Vertex), (void*)offsetof(Data::Vertex, Data::Vertex::uvs));
                // vertex normals
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Data::Vertex), (void*)offsetof(Data::Vertex, Data::Vertex::normals));
                // vertex colors
                glEnableVertexAttribArray(3);
                glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Data::Vertex), (void*)offsetof(Data::Vertex, Data::Vertex::colors));

                glBindVertexArray(0);
            }

            void Mesh::Render()
            {
                glBindVertexArray(this->vao);
                glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
                glDrawElements(GL_TRIANGLES, this->meshData.indices.size(), GL_UNSIGNED_INT, (void*)0);
            }
        }
    }
}
