#ifndef _CUBE_H
#define _CUBE_H

#pragma once

#include "../../Game.h"

class Cube : public GameObject
{
public:
	int width = 1;
	int height = 1;
	int depth = 1;

	Texture* texture;
	Shader* shader;
	Model* model;

	Cube(glm::vec3 pos, float _w, float _h, float _d, Model* m, Texture* _sprSheet);
	void Draw() override;
};

#endif