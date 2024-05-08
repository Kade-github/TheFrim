#ifndef _INVENTORY_H
#define _INVENTORY_H

#include "BoxUI.h"
#include "ItemUI.h"
#include "../Player.h"

class Inventory : public BoxUI
{
	Player* player;
	bool _dragging = false;
	glm::vec2 _startDrag;
	int _draggingSlot = -1;
	ItemUI* _draggingItem = nullptr;

	Sprite2D* furnace_progress = nullptr;

public:

	Data::InventoryItem crafting[9]; 
	Data::InventoryItem stored_crafting[3][3];
	Data::InventoryItem output;

	Data::InventoryItem furnace_cooking;
	Data::InventoryItem furnace_fuel;

	Data::BlockData furnace;

	Data::InventoryItem stored;

	bool shown = false;

	Inventory(glm::vec3 _pos, Player* _player);

	bool isFurnace = false;
	bool isThreeByThree = false;
	bool isArmor = false;

	void SetCrafting();
	void UpdateTable();

	void CreateInventory();

	void CreateArmor();

	void CreateTwoByTwoCrafting();

	void CreateThreeByThreeCrafting();

	void CreateFurnace();

	void UpdateInventory(bool dontRemoveOutput = false);

	glm::vec2 ConvertToSlotPos(std::string tag_id);

	void ApplyMove(Data::InventoryItem* item1, Data::InventoryItem* item2);

	Data::InventoryItem* GetItem(int id, glm::vec2 pos);

	void Close();

	bool SwitchItem(glm::vec3 from, glm::vec3 to, bool one = false);

	void MouseClick(int button, glm::vec2 pos) override;
	void MouseRelease(int button, glm::vec2 pos) override;

	void Draw() override;
};

#endif // _INVENTORY_H