/*
	Copyright (c) 2023-2024 Kade

	Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _VERTEXDATA_H_
#define _VERTEXDATA_H_

#include "../../../Includes.h"

#include "../../../include/glm/vec2.hpp"
#include "Color.h"

namespace Engine
{
	namespace Data
	{
		struct Vertex2D {
			std::vector<glm::vec2> uvs; // 2D UVs (normalized)
			std::vector<glm::vec2> vertices; // 2D Positions (normalized)
			Color color; // Color (RGBA)

			Vertex2D() {

			}

			Vertex2D(std::vector<glm::vec2> vertice, std::vector<glm::vec2> uv, Color _color) {
				this->vertices = vertices;
				this->uvs = uv;
				this->color = _color;
			}
		};

		struct VertexData
		{
		public:

			std::vector<Vertex2D> vertexs;

			VertexData() {

			}

			VertexData(std::vector<Vertex2D> vertices) {
				this->vertexs = vertices;
			}

		};
	}
}

#endif