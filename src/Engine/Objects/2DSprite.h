#ifndef _2DSPRITE_H
#define _2DSPRITE_H

#include "2DGameObject.h"

class Sprite2D : public GameObject2D
{
	bool _noDelete = false;
public:
	glm::vec4 src = glm::vec4(0,0,1,1);

	bool flipHorizontal = false;

	float width, height;
	Sprite2D(glm::vec3 _pos);
	Sprite2D(std::string texture, glm::vec3 _pos);
	Sprite2D(Texture* t, glm::vec3 _pos);

	void Resize(float _w, float _h);

	void Draw() override;
};

#endif