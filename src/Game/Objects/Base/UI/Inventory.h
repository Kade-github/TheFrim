#ifndef _INVENTORY_H
#define _INVENTORY_H

#include "BoxUI.h"
#include "../Player.h"

class Inventory : public BoxUI
{
	Player* player;
public:

	bool shown = false;

	Inventory(glm::vec3 _pos, Player* _player);

	void UpdateInventory();

	void MouseClick(int button, glm::vec2 pos) override;
	void MouseRelease(int button, glm::vec2 pos) override;

	void Draw() override;
};

#endif // _INVENTORY_H