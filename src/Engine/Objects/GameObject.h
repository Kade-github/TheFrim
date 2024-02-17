#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#pragma once

#include "../../../Include/glm/vec3.hpp"
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

	virtual void Draw() {};
};

#endif