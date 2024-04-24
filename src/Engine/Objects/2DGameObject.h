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
	float r, g, b, a;

	Vertex2D(float _x, float _y, float _z, float _u, float _v, float _r, float _g, float _b, float _a)
	{
		x = _x;
		y = _y;
		z = _z;
		u = _u;
		v = _v;
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	Vertex2D(glm::vec2 _pos, glm::vec2 _uv, glm::vec4 _color)
	{
		x = _pos.x;
		y = _pos.y;
		z = 0;
		u = _uv.x;
		v = _uv.y;
		r = _color.r;
		g = _color.g;
		b = _color.b;
		a = _color.a;
	}
};

struct Draw2D
{
	Texture* textureId;
	Shader* shaderId;

	std::vector<Vertex2D> vertices;

	glm::vec4 clipRect = glm::vec4(-1, -1, -1, -1);
};


class GameObject2D : public GameObject
{
public:
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 clip = glm::vec4(-1, -1, -1, -1);

	GameObject2D(glm::vec3 pos) : GameObject(pos)
	{

	}

	~GameObject2D()
	{
		draws = {};
	}

	Texture* t = NULL;
	Shader* s = NULL;

	std::vector<Draw2D> draws = {};
};

#endif