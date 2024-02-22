#ifndef _2DGAMEOBJECT_H
#define _2DGAMEOBJECT_H

#include "GameObject.h"
#include <vector>
#include "../OpenGL/Texture.h"
#include "../OpenGL/Shader.h"

struct Vertex2D
{
	glm::vec3 position;
	glm::vec2 texCoords;
};

struct Draw2D
{
	Texture* textureId;
	Shader* shaderId;

	std::vector<Vertex2D> vertices;
};


class GameObject2D : public GameObject
{
public:
	Texture* t = NULL;
	Shader* s = NULL;

	std::vector<Draw2D> draws = {};
};

#endif