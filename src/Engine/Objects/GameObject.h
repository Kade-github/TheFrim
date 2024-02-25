#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#pragma once

#include "../../../Include/glm/gtc/type_ptr.hpp"
#include "../../../Include/glm/gtc/matrix_transform.hpp"

class GameObject
{
public:
	GameObject(glm::vec3 pos)
	{
		position = pos;
	}

	bool visible = true;

	int order = 0;
	int id = -1;

	glm::vec3 position = {};

	glm::vec3 rotateAxis = glm::vec3(0.5, 0.5, 0.5);
	float angle = 0;

	virtual void Draw() {};

	virtual void Create() {};

	virtual void Destroy() {};
};

#endif