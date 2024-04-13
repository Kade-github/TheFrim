#ifndef _INVENTORY_H
#define _INVENTORY_H

#include "BoxUI.h"
#include "../Player.h"

class Inventory : public BoxUI
{
	Player* player;
	bool _dragging = false;
	glm::vec2 _startDrag;
	Sprite2D* _draggingItem = nullptr;

public:

	Data::InventoryItem crafting[4]; 
	Data::InventoryItem output;

	bool shown = false;

	Inventory(glm::vec3 _pos, Player* _player);

	void UpdateInventory();

	void ApplyMove(Data::InventoryItem* item1, Data::InventoryItem* item2);

	Data::InventoryItem* GetItem(int id, glm::vec2 pos);

	void Close();

	bool SwitchItem(glm::vec3 from, glm::vec3 to);

	void MouseClick(int button, glm::vec2 pos) override;
	void MouseRelease(int button, glm::vec2 pos) override;

	void Draw() override;
};

#endif // _INVENTORY_H