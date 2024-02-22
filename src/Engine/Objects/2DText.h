#ifndef _2DTEXT_H
#define _2DTEXT_H

#include "2DGameObject.h"
#include "Font/Fnt.h"

class Text2D : public GameObject2D
{
public:
	std::string text = "";
	Fnt* font = NULL;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	float scale = 1.0f;

	Text2D(std::string text, Fnt* font, glm::vec3 pos, glm::vec3 color, float scale = 1.0f);

	void Draw() override;
};

#endif