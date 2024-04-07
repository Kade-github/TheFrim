#ifndef _DROPPED_ITEM_MANAGER_H
#define _DROPPED_ITEM_MANAGER_H

#include <vector>
#include "Objects/Base/DroppedItem.h"

class DroppedItemManager
{
public:
	std::vector<DroppedItem*> items;

	~DroppedItemManager();

	void SpawnItem(glm::vec3 pos, glm::vec3 front, Data::InventoryItem i);
	void SpawnItem(glm::vec3 pos, Data::InventoryItem i);

	void AddItem(DroppedItem* item);

	void RemoveItem(DroppedItem* item);

	void Update();
};

#endif