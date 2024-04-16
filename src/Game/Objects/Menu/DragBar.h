#ifndef _DRAG_BAR_H
#define _DRAG_BAR_H

#include <Objects/2DText.h>
#include <Objects/2DSprite.h>

class DragBar : public Sprite2D
{
	Sprite2D* drag;
	Text2D* t;

	std::string _initialText;

	glm::vec2 initialPos;
	float _initialValue;
public:
	float value = 0.0f;
	float max = 1.0f;
	DragBar(glm::vec3 _position, std::string _text, float _value);

	void Draw() override;
	void MouseClick(int button, glm::vec2 mPos) override;
	void MouseRelease(int button, glm::vec2 mPos) override;

	void SetText(std::string _text);

	std::string text;
};

#endif