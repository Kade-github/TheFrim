#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#pragma once

#include "Camera.h"

class GameObject
{
public:
	GameObject(glm::vec3 pos)
	{
		position = pos;
	}

	int id = 0;

	Camera* camera;
	glm::vec3 position = {};

	glm::vec3 rotateAxis = glm::vec3(0.5, 0.5, 0.5);
	float angle = 0;

	virtual void Draw() {};
};

#endif