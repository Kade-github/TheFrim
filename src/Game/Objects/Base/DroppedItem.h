#ifndef _DROPPED_ITEM_H
#define _DROPPED_ITEM_H

#include "../../Data/Player.h"
#include <Objects/3DSprite.h>
#include "Entity.h"

class DroppedItem : public Entity
{
	bool _stackable = true;
public:
	Data::InventoryItem item;
	float lifeTime = 0;
	Sprite3D* sprite;
	DroppedItem(glm::vec3 _pos, Texture* t, Data::InventoryItem i, bool stackable = true);


	void Draw() override;

};
#endif