#ifndef _CUBE_H
#define _CUBE_H

#pragma once

#include "../Scene.h"
#include "../../OpenGL/Texture.h"

class Cube : public GameObject
{
public:
	int width = 1;
	int height = 1;
	int depth = 1;

	Texture* texture;

	Cube(glm::vec3 pos, float _w, float _h, float _d, Texture* _sprSheet);
	void Draw() override;
};

#endif