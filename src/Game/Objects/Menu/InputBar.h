#ifndef _INPUT_BAR_H
#define _INPUT_BAR_H

#include <Objects/2DSprite.h>
#include <Objects/2DText.h>

class InputBar : public Sprite2D
{
	Text2D* _text;
public:
	bool selected = false;

	std::string text = "";

	int maxChars = 26;

	InputBar(glm::vec3 _pos);
    ~InputBar();

	void Draw() override;

	void AddText(char _c);
	void RemoveText();
};

#endif