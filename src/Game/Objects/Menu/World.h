#ifndef _OBJWORLD_H
#define _OBJWORLD_H

#include <Objects/2DSprite.h>

class World : public Sprite2D
{
public:
	World(glm::vec3 _pos);
};

#endif