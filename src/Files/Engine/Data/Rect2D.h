/*
	Copyright (c) 2023-2024 Kade

	Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _RECT2D_H_
#define _RECT2D_H_

#include "VertexData.h"

namespace Engine
{
	namespace Data
	{
		struct Rect2D {
        public:
            Rect2D(float _x, float _y) : x(_x), y(_y) {}
            Rect2D(float _x, float _y, float _width, float _height) : x(_x), y(_y), width(_width), height(_height) {}
            Rect2D(float _x, float _y, float _width, float _height, float _r, float _g, float _b, float _a) : x(_x), y(_y), width(_width), height(_height), r(_r), g(_g), b(_b), a(_a) {}
            Rect2D(float _x, float _y, float _width, float _height, float _r, float _g, float _b, float _a, float _sX, float _sY) : x(_x), y(_y), width(_width), height(_height), r(_r), g(_g), b(_b), a(_a), src_x(_sX), src_y(_sY) {}
            float x, y;
            float width, height;
            float r, g, b, a;
            float src_x, src_y;

            VertexData ToData()
            {
                VertexData d;
                Vertex2D v2;
                v2.color = Color(r,g,b,a);
                v2.uvs = {glm::vec2(src_x,src_y)};
                v2.vertices = {};

                // create a quad through triangles

                glm::vec2 topLeft = glm::vec2(x,y);
                glm::vec2 topRight = glm::vec2(x + width, y);
                glm::vec2 bottomLeft = glm::vec2(x, y + height);
                glm::vec2 bottomRight = glm::vec2(x + width, y + height);

                v2.vertices.push_back(topLeft);
                v2.vertices.push_back(topRight);
                v2.vertices.push_back(bottomLeft);
                v2.vertices.push_back(bottomLeft);
                v2.vertices.push_back(topRight);
                v2.vertices.push_back(bottomRight);

                return d;
            }
        };
	}
}

#endif