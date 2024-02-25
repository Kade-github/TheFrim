#ifndef _OBJWORLD_H
#define _OBJWORLD_H

#include <Objects/2DSprite.h>
#include <Objects/2DText.h>

class World : public Sprite2D
{
	Text2D* label_name;
	Text2D* label_seed;
public:
	Sprite2D* worldThumbnail;
	World(glm::vec3 _pos);

	void SetName(std::string _name);
	void SetSeed(std::string _seed);

	void Draw() override;
};

#endif