#ifndef _2DTEXT_H
#define _2DTEXT_H

#include "2DGameObject.h"
#include "Font/Fnt.h"

struct RenderedCharacter
{
	float w;
	float h;
	glm::vec4 src;
	bool space = false;
	int advance;
};

struct Line
{
	int w;
	std::vector<RenderedCharacter> characters;
};

class Text2D : public GameObject2D
{
public:
	bool alienMode = false;
	std::string text = "";
	Fnt* font = NULL;
	int size = 12;

	bool wrap = false;
	bool center = false;

	Text2D(std::string text, std::string font, glm::vec3 pos, glm::vec4 color, int size = 12);

	void Draw() override;
};

#endif