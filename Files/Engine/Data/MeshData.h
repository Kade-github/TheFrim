/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _MESHDATA_H_
#define _MESHDATA_H_

#include "../../../Includes.h"

#include "../../../include/glm/vec2.hpp"
#include "../../../include/glm/vec3.hpp"
#include "Color.h"

namespace Engine
{
	namespace Data
	{

		struct Vertex {
			std::vector<glm::vec3> vertices; // 3D vertice coordinates
			std::vector<glm::vec3> normals; // 3D normal coordinates
			std::vector<glm::vec2> uvs; // 2D texture coordinates
			std::vector<Color> colors; // Colors (RGBA)

			Vertex() {
			
			}

			Vertex(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs) {
				this->vertices = vertices;
				this->normals = normals;
				this->uvs = uvs;
			}
		};

		struct MeshData
		{
		public:

			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;


			MeshData() {
			
			}

			MeshData(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
				this->vertices = vertices;
				this->indices = indices;
			}

		};
	}
}

#endif