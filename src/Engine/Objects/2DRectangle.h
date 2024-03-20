#ifndef _2DRECTANGLE_H
#define _2DRECTANGLE_H

#include "2DGameObject.h"

class Rectangle2D : public GameObject2D
{
public:
	glm::vec4 color;

	float width;
	float height;

	Rectangle2D(glm::vec3 pos, glm::vec2 size, glm::vec4 color) : GameObject2D(pos)
	{
		width = size.x;
		height = size.y;
		this->color = color;
	}

	void Draw()
	{
		glm::vec3 p = position;

		std::vector<Vertex2D> vertices;

		Vertex2D tl = { p, glm::vec2{0,0}, color };
		Vertex2D tr = { p + glm::vec3{width, 0, 0}, glm::vec2{1,0}, color };
		Vertex2D bl = { p + glm::vec3{0, height, 0}, glm::vec2{0,1}, color };
		Vertex2D br = { p + glm::vec3{width, height, 0}, glm::vec2{1,1}, color };

		vertices.push_back(tl);
		vertices.push_back(tr);
		vertices.push_back(bl);
		vertices.push_back(bl);
		vertices.push_back(tr);
		vertices.push_back(br);

		draws = { { NULL, s, vertices, clip } };
	}
};

#endif