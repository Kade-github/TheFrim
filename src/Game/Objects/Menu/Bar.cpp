#include "Bar.h"

Bar::Bar(glm::vec3 _pos, std::string text) : Sprite2D("Assets/Textures/MainMenu/bar.png", _pos)
{
    label = new Text2D(text, "ArialFrim", _pos, glm::vec4(1, 1, 1, 1), 62);

}

void Bar::SetText(std::string text)
{
    label->text = text;
}

void Bar::Draw()
{
    label->size = 62 * (height / t->height);
    label->position.x = position.x + (width / 2) - (label->width / 2);
    label->position.y = position.y;

    label->color = color;

    Sprite2D::Draw();
	label->Draw();
    draws = { draws[0], label->draws[0]};
}

void Bar::Destroy() {
    delete label;
    GameObject::Destroy();
}