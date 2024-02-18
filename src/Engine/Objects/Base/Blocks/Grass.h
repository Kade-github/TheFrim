#ifndef _GRASS_H
#define _GRASS_H

#pragma once

#include "../Cube.h"

class Grass : public Cube
{
public:
	Grass(glm::vec3 pos, float _w, float _h, float _d, Model* m, Texture* _sprSheet) : Cube(pos, _w, _h, _d, m, _sprSheet)
	{

	}

	void Draw() override
	{
		glm::vec4 dirt = texture->getSrc(0, 256);
		glm::vec4 grass = texture->getSrc(0, 128);
		glm::vec4 side = texture->getSrc(0, 0);

		// set all to dirt

		int i = 0;
		for (auto& v : model->meshes[0].vertices)
		{
			int x = i % 4;
			switch (x)
			{
				case 0:
					v.texCoords = glm::vec2(dirt.x, dirt.y);
					break;
				case 1:
					v.texCoords = glm::vec2(dirt.x + dirt.z, dirt.y);
					break;
				case 2:
					v.texCoords = glm::vec2(dirt.x, dirt.y + dirt.w);
					break;
				case 3:
					v.texCoords = glm::vec2(dirt.x + dirt.z, dirt.y + dirt.w);
					break;
			}
			i++;
		}

		Cube::Draw();
	}
};

#endif