#include "Inventory.h"
#include "ItemUI.h"

Inventory::Inventory(glm::vec3 _pos, Player* _player) : BoxUI(_pos, 11, 6)
{
	player = _player;
	order = 100;
}

void Inventory::UpdateInventory()
{
	ClearFront();

	Texture* i = Texture::createWithImage("Assets/Textures/items.png", false); // grab from cache
	for (int y = 0; y < PLAYER_INVENTORY_HEIGHT; y++)
	{
		for (int x = 0; x < PLAYER_INVENTORY_WIDTH; x++)
		{
			Data::InventoryItem& item = player->playerData.inventory[x][y];

			if (item.type == Data::ItemType::ITEM_NULL)
				continue;

			int rX = x + 1;
			int rY = y + 1;

			ItemUI* s = new ItemUI(item.tag, glm::vec3(0, 0, 0), i, item.count);

			AddFront(s, rX, rY);
		}
	}
}

void Inventory::MouseClick(int button, glm::vec2 pos)
{

}

void Inventory::MouseRelease(int button, glm::vec2 pos)
{
}

void Inventory::Draw()
{
	draws.clear();

	if (!shown)
		return;


	BoxUI::Draw();
}
