#include "Inventory.h"
#include "ItemUI.h"
#include <Helpers/StringTools.h>
#include <Game.h>
#include "../../../Scenes/Gameplay.h"

Inventory::Inventory(glm::vec3 _pos, Player* _player) : BoxUI(_pos, 11, 10)
{
	player = _player;
	order = 100;

	int i = 0;

	// 9x4

	for (int y = 0; y < PLAYER_INVENTORY_HEIGHT; y++)
	{
		for (int x = 0; x < PLAYER_INVENTORY_WIDTH; x++)
		{
			AddSlot(x + 1, PLAYER_INVENTORY_HEIGHT - y, i);
			i++;
		}
	}

	// i at this point is = 36

	// armor

	for (int y = 2; y < 5; y++) // 36, 37, 38
	{
		AddSlot(2, PLAYER_INVENTORY_HEIGHT + y, i);
		i++;
	}

	// crafting

	for (int y = 1; y < 3; y++) // 39, 40, 41, 42
	{
		for (int x = 1; x < 3; x++)
		{
			AddSlot(x + 4, PLAYER_INVENTORY_HEIGHT + y + 1.5, i);
			i++;
		}
	}

	// output

	AddSlot(8, PLAYER_INVENTORY_HEIGHT + 3, i); // 43

}

void Inventory::UpdateInventory()
{
	ClearFronts();

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

			glm::vec3 pos = position + glm::vec3(64 * rX, 64 * rY, 0);

			BoxSlot& sl = GetSlot(pos);

			if (sl.id == -1)
				continue;

			ItemUI* s = new ItemUI(item.tag, pos, i, item.count);

			s->width = 32;
			s->height = 32;

			sl.front = s;
		}
	}

	// armor
	int id = 36;
	for (int y = 2; y < 5; y++) // 36, 37, 38
	{
		Data::InventoryItem& item = player->playerData.armor[y - 2];

		if (item.type == Data::ItemType::ITEM_NULL)
			continue;

		BoxSlot& sl = GetSlot(id);

		if (sl.id == -1)
			continue;

		glm::vec3 pos = position + glm::vec3(64 * sl.x, 64 * sl.y, 0);

		ItemUI* s = new ItemUI(item.tag, pos, i, item.count);

		s->width = 32;
		s->height = 32;

		sl.front = s;
		i++;
	}

	// crafting

	id = 39;
	for (int y = 1; y < 3; y++) // 39, 40, 41, 42
	{
		for (int x = 1; x < 3; x++)
		{
			Data::InventoryItem& item = crafting[(y - 1) * 2 + (x - 1)];

			if (item.type == Data::ItemType::ITEM_NULL)
				continue;

			BoxSlot& sl = GetSlot(id);

			if (sl.id == -1)
				continue;

			glm::vec3 pos = position + glm::vec3(64 * sl.x, 64 * sl.y, 0);

			ItemUI* s = new ItemUI(item.tag, pos, i, item.count);

			s->width = 32;
			s->height = 32;

			sl.front = s;
			i++;
		}
	}
}

void Inventory::ApplyMove(Data::InventoryItem* item1, Data::InventoryItem* item2)
{
	if (item2->type == Data::ItemType::ITEM_NULL)
	{
		Data::InventoryItem temp = *item1;

		*item1 = *item2;
		*item2 = temp;
		
	}
	else
	{
		if (item1->type == item2->type && item2->stackable)
		{
			if (item1->count > 64)
				item1->count = 64;

			if (item2->count > 64)
				item2->count = 64;

			float newCount = item1->count + item2->count;

			if (newCount > 64)
			{
				item1->count = newCount - 64;
				item2->count = 64;
			}
			else
			{
				item2->count = newCount;
				item1 = {};
			}
		}
		else
		{
			Data::InventoryItem temp = *item1;

			*item1 = *item2;
			*item2 = temp;
			
		}
	}
}

void Inventory::Close()
{
	// give back all crafting

	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	for (int i = 0; i < 4; i++)
	{
		Data::InventoryItem item = crafting[i];

		if (!player->playerData.GiveItem(item)) // inventory full, so drop it
		{
			Camera* c = Game::instance->GetCamera();

			gp->dim->SpawnItem(player->position + c->cameraFront, c->cameraFront, item);
		}

		crafting[i] = {};
	}

	gp->hud->UpdateHotbar();
}

bool Inventory::SwitchItem(glm::vec3 from, glm::vec3 to)
{
	BoxSlot& s = GetSlot(to); // to
	BoxSlot& sSlot = GetSlot(from); // from
	Sprite2D* slot = s.slot;
	Sprite2D* startSlot = sSlot.slot;

	_dragging = false;

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

		Data::InventoryItem* startItem = nullptr;
		Data::InventoryItem* endItem = nullptr;

		if (sSlot.id < 36)
			startItem = &player->playerData.inventory[(int)start.x][(int)start.y];
		else if (sSlot.id < 39)
			startItem = &player->playerData.armor[sSlot.id - 36];
		else if (sSlot.id < 43)
			startItem = &crafting[sSlot.id - 39];
		else if (sSlot.id == 43)
			startItem = &output;

		if (s.id < 36)
			endItem = &player->playerData.inventory[(int)end.x][(int)end.y];
		else if (s.id < 39)
			endItem = &player->playerData.armor[s.id - 36];
		else if (s.id < 43)
			endItem = &crafting[s.id - 39];
		else if (s.id == 43)
			return false; // can't move to output

		// check if we can move it to that slot (armor)

		if (s.id >= 36 && s.id < 39)
		{
			int slot = s.id - 36;
			if (!startItem->equipable || slot != startItem->armorSlot)
				return false;
		}

		ApplyMove(startItem, endItem);
	}
	else
	{
		if (to.x < position.x || to.x > position.x + renderWidth || to.y < position.y || to.y > position.y + renderHeight)
		{
			// drop item
			glm::vec3 from = glm::vec3(_startDrag.x, _startDrag.y, 0);

			std::vector<std::string> sPos = StringTools::Split(startSlot->tag_id, ",");

			glm::vec2 start = glm::vec2(std::stoi(sPos[0]), std::stoi(sPos[1]));

			start.x -= 1;
			start.y = PLAYER_INVENTORY_HEIGHT - start.y;

			Data::InventoryItem* startItem = nullptr;

			if (sSlot.id < 36)
				startItem = &player->playerData.inventory[(int)start.x][(int)start.y];
			else if (sSlot.id < 39)
				startItem = &player->playerData.armor[sSlot.id - 36];
			else if (sSlot.id < 43)
				startItem = &crafting[sSlot.id - 39];
			else if (sSlot.id == 43)
				startItem = &output;

			if (startItem == nullptr || startItem->type == Data::ItemType::ITEM_NULL)
				return false;

			Data::InventoryItem item = *startItem;

			Camera* c = Game::instance->GetCamera();

			Gameplay* gp = (Gameplay*)Game::instance->currentScene;

			gp->dim->SpawnItem(player->position + c->cameraFront, c->cameraFront, item);	

			*startItem = {};

			return true;
		}

		return false;
	}

	return true;
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
			glm::vec3 from = glm::vec3(_startDrag.x, _startDrag.y, 0);
			glm::vec3 to = glm::vec3(pos.x, pos.y, 0);

			if (!SwitchItem(from, to))
			{
				BoxSlot& sSlot = GetSlot(from);

				_draggingItem->position = glm::vec3(from.x * sSlot.front->width, from.y * sSlot.front->height, 0);
			}

			UpdateInventory();

			Gameplay* gp = (Gameplay*)Game::instance->currentScene;

			gp->hud->UpdateHotbar();
		}

		_dragging = false;
		_draggingItem = nullptr;
	}
}

void Inventory::Draw()
{
	draws.clear();

	if (!shown)
		return;

	glm::vec2 mouse = Game::instance->GetCursorPos();

	// selection

	BoxSlot& s = GetSlot(mouse);

	Sprite2D* selectedSlot = s.slot;

	for (BoxSlot& _s : boxSlots)
	{
		_s.slot->src = t->spriteSheet.GetUVFlip("box_slot");
	}

	if (selectedSlot != nullptr && s.id != -1)
		selectedSlot->src = t->spriteSheet.GetUVFlip("box_slot_selected");

	if (_dragging)
		_draggingItem->position = glm::vec3(mouse.x - _draggingItem->width / 2, mouse.y - _draggingItem->height / 2, 0);

	BoxUI::Draw();
}
