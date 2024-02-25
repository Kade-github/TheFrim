#include "InputBar.h"

InputBar::InputBar(glm::vec3 _pos) : Sprite2D("Assets/Textures/MainMenu/inputBar.png", _pos)
{
	_text = new Text2D("", "ArialFrim", glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1), 62);
}

void InputBar::Draw()
{
    _text->size = 62 * (height / t->height);
    _text->position.x = position.x + 4;
    _text->position.y = position.y;

    Sprite2D::Draw();
    _text->Draw();
}
