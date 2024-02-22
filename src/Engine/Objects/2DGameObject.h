#ifndef _2DGAMEOBJECT_H
#define _2DGAMEOBJECT_H

#include "GameObject.h"
#include <vector>
#include "../OpenGL/Texture.h"
#include "../OpenGL/Shader.h"

struct Vertex2D
{
public:
	float x, y, z;
	float u, v;

	Vertex2D(float _x, float _y, float _z, float _u, float _v)
	{
		x = _x;
		y = _y;
		z = _z;
		u = _u;
		v = _v;
	}
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
	GameObject2D(glm::vec3 pos) : GameObject(pos)
	{

	}

	Texture* t = NULL;
	Shader* s = NULL;

	std::vector<Draw2D> draws = {};
};

#endif