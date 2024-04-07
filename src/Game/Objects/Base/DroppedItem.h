#ifndef _DROPPED_ITEM_H
#define _DROPPED_ITEM_H

#include "../../Data/Player.h"
#include <Objects/3DSprite.h>
#include "Entity.h"

class DroppedItem : public Entity
{
public:
	Data::InventoryItem item;
	float lifeTime = 0;
	Sprite3D* sprite;
	DroppedItem(glm::vec3 _pos, Texture* t, Data::InventoryItem i);


	void Draw() override;

};
#endif