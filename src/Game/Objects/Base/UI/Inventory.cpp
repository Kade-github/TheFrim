#include "Inventory.h"
#include "ItemUI.h"
#include <Helpers/StringTools.h>
#include <Game.h>
#include "../../../Scenes/Gameplay.h"

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
			int rY = PLAYER_INVENTORY_HEIGHT - y;

			ItemUI* s = new ItemUI(item.tag, glm::vec3(0, 0, 0), i, item.count);

			AddFront(s, rX, rY);
		}
	}
}

void Inventory::MouseClick(int button, glm::vec2 pos)
{
	if (!shown)
		return;

	if (button == 0)
	{
		_draggingItem = GetFront(pos);

		if (_draggingItem != nullptr)
		{
			_dragging = true;
			_startDrag = pos;
		}

	}
}

void Inventory::MouseRelease(int button, glm::vec2 pos)
{
	if (button == 0)
	{
		if (_dragging)
		{
			Sprite2D* slot = GetSlot(pos);
			Sprite2D* startSlot = GetSlot(_startDrag);

			if (slot != nullptr && startSlot != nullptr)
			{
				// 9x4

				std::vector<std::string> sPos = StringTools::Split(startSlot->tag_id, ",");

				glm::vec2 start = glm::vec2(std::stoi(sPos[0]), std::stoi(sPos[1]));

				sPos = StringTools::Split(slot->tag_id, ",");
				glm::vec2 end = glm::vec2(std::stoi(sPos[0]), std::stoi(sPos[1]));

				start.x -= 1;
				end.x -= 1;

				start.y = PLAYER_INVENTORY_HEIGHT - start.y;
				end.y = PLAYER_INVENTORY_HEIGHT - end.y;

				Data::InventoryItem startItem = player->playerData.inventory[(int)start.x][(int)start.y];
				Data::InventoryItem endItem = player->playerData.inventory[(int)end.x][(int)end.y];

				if (startItem.type != endItem.type)
				{
					player->playerData.inventory[(int)start.x][(int)start.y] = endItem;
					player->playerData.inventory[(int)end.x][(int)end.y] = startItem;
				}
				else
				{
					if (startItem.type != Data::ItemType::ITEM_NULL)
					{
						int total = startItem.count + endItem.count;

						if (total > 64)
						{
							int diff = total - 64;

							player->playerData.inventory[(int)start.x][(int)start.y].count = diff;
							player->playerData.inventory[(int)end.x][(int)end.y].count = 64;
						}
						else
						{
							player->playerData.inventory[(int)start.x][(int)start.y] = {};
							player->playerData.inventory[(int)end.x][(int)end.y].count = total;
						}
					}
					else
					{
						player->playerData.inventory[(int)start.x][(int)start.y] = endItem;
						player->playerData.inventory[(int)end.x][(int)end.y] = startItem;
					}
				}
			}

			UpdateInventory();

			Gameplay* gp = (Gameplay*)Game::instance->currentScene;

			gp->hud->UpdateHotbar();
		}

		_dragging = false;
	}
}

void Inventory::Draw()
{
	draws.clear();

	if (!shown)
		return;

	glm::vec2 mouse = Game::instance->GetCursorPos();

	// selection

	Sprite2D* selectedSlot = GetSlot(mouse);

	for (Sprite2D* s : GetSlots())
		s->src = t->spriteSheet.GetUVFlip("box_slot");

	if (selectedSlot != nullptr)
		selectedSlot->src = t->spriteSheet.GetUVFlip("box_slot_selected");

	if (_dragging)
	{
		_draggingItem->position = glm::vec3(mouse.x - _draggingItem->width / 2, mouse.y - _draggingItem->height / 2, 0);
	}


	BoxUI::Draw();
}
