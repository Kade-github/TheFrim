#ifndef _DROPPED_ITEM_H
#define _DROPPED_ITEM_H

#include "../../Data/Player.h"
#include <Objects/3DSprite.h>
#include "Entity.h"

class DroppedItem : public Entity
{
	unsigned int VAO, VBO, EBO;

	std::vector<GameObject::VVertex> vertices;
	std::vector<unsigned int> indices;

	Texture* txp;

public:

	Data::InventoryItem item;
	float lifeTime = 0;
	bool blockMode = false;
	Sprite3D* sprite;
	DroppedItem(glm::vec3 _pos, Texture* t, Data::InventoryItem i);

	~DroppedItem();

	void Draw() override;

};
#endif