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

    _text->text = text;
    _text->clip = glm::vec4(position.x, position.y, width, height);

    if (selected)
        _text->text = text + "_";

    Sprite2D::Draw();
    _text->Draw();
}

void InputBar::AddText(char _c)
{
    int max = std::floor(t->width / _text->size);
    if (text.size() < max)
        text += _c;

}

void InputBar::RemoveText()
{
	if (text.size() > 0)
		text.pop_back();
}
