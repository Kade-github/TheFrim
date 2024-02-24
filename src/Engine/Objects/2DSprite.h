#ifndef _2DSPRITE_H
#define _2DSPRITE_H

#include "2DGameObject.h"

class Sprite2D : public GameObject2D
{
public:
	float width, height;
	Sprite2D(std::string texture, glm::vec3 _pos);

	~Sprite2D();

	void Resize(float _w, float _h);

	void Draw() override;
};

#endif