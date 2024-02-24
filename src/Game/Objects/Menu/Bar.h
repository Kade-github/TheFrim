#ifndef _BAR_H
#define _BAR_H

#include <Objects/2DSprite.h>
#include <Objects/2DText.h>

class Bar : public Sprite2D
{
public:
    Text2D* label;

    bool selected = false;

    Bar(glm::vec3 _pos, std::string text);

    void Draw() override;
};

#endif