#include "Bar.h"

Bar::Bar(glm::vec3 _pos, std::string text) : Sprite2D("Assets/Textures/MainMenu/bar.png", _pos)
{
    label = new Text2D(text, "ArialFrim", _pos, glm::vec4(1, 1, 1, 1), 62);
    label->center = true;

}

void Bar::Draw()
{
    label->size = 62 * (height / t->height);
    label->position.x = position.x + (width / 2);
    label->position.y = position.y + 8;

    Sprite2D::Draw();
	label->Draw();
    draws = { draws[0], label->draws[0]};
}