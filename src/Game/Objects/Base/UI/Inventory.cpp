#include "Inventory.h"
#include <Helpers/StringTools.h>
#include <Game.h>
#include "../../../CraftingManager.h"
#include "../../../Scenes/Gameplay.h"
#include "../Blocks/Furnace.h"


Inventory::Inventory(glm::vec3 _pos, Player* _player) : BoxUI(_pos, 11, 10)
{
	player = _player;
	order = 100;
}

void Inventory::SetCrafting()
{
	for (auto& arr : stored_crafting)
		std::fill(std::begin(arr), std::end(arr), Data::InventoryItem());

	if (isThreeByThree)
	{
		stored_crafting[0][0] = crafting[6];
		stored_crafting[0][1] = crafting[7];
		stored_crafting[0][2] = crafting[8];

		stored_crafting[1][0] = crafting[3];
		stored_crafting[1][1] = crafting[4];
		stored_crafting[1][2] = crafting[5];

		stored_crafting[2][0] = crafting[0];
		stored_crafting[2][1] = crafting[1];
		stored_crafting[2][2] = crafting[2];


	}
	else
	{
		stored_crafting[0][0] = crafting[2];
		stored_crafting[0][1] = crafting[3];

		stored_crafting[1][0] = crafting[0];
		stored_crafting[1][1] = crafting[1];
	}
}

void Inventory::UpdateTable()
{
	if (isThreeByThree)
	{
		crafting[6] = stored_crafting[0][0];
		crafting[7] = stored_crafting[0][1];
		crafting[8] = stored_crafting[0][2];

		crafting[3] = stored_crafting[1][0];
		crafting[4] = stored_crafting[1][1];
		crafting[5] = stored_crafting[1][2];

		crafting[0] = stored_crafting[2][0];
		crafting[1] = stored_crafting[2][1];
		crafting[2] = stored_crafting[2][2];

	}
	else
	{
		crafting[2] = stored_crafting[0][0];
		crafting[3] = stored_crafting[0][1];

		crafting[0] = stored_crafting[1][0];
		crafting[1] = stored_crafting[1][1];
	}
}

void Inventory::CreateInventory()
{
	ClearSlots();

	furnace_cooking = {};
	furnace_fuel = {};

	isFurnace = false;
	isThreeByThree = false;
	isArmor = false;

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
}

void Inventory::CreateArmor()
{
	int i = 36;

	isArmor = true;

	// armor

	for (int y = 2; y < 5; y++) // 36, 37, 38
	{
		AddSlot(2, PLAYER_INVENTORY_HEIGHT + y, i);
		i++;
	}

}

void Inventory::CreateTwoByTwoCrafting()
{
	int i = 39;
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

	AddSlot(8, PLAYER_INVENTORY_HEIGHT + 3, 90); // 90
}

void Inventory::CreateThreeByThreeCrafting()
{
	int i = 39;

	isThreeByThree = true;

	// crafting

	/*
		39 40 41
		42 43 44
		45 46 47
	*/

	for (int y = 1; y < 4; y++) 
	{
		for (int x = 1; x < 4; x++)
		{
			AddSlot(x + 2, PLAYER_INVENTORY_HEIGHT + y + 1, i);
			i++;
		}
	}

	// output

	AddSlot(7.5f, PLAYER_INVENTORY_HEIGHT + 3, 90); // 90
}

void Inventory::CreateFurnace()
{
	int i = 48;

	isFurnace = true;

	// two slots

	AddSlot(3.5f, PLAYER_INVENTORY_HEIGHT + 1.5f, i); // 48

	AddSlot(3.5f, PLAYER_INVENTORY_HEIGHT + 3.5f, i + 1); // 49

	// output

	AddSlot(6.5f, PLAYER_INVENTORY_HEIGHT + 2.5f, 90); // 90

	if (player->selectedBlock != nullptr)
	{
		furnace = player->selectedBlock->data;


		furnace_progress = new Sprite2D("Assets/Textures/ui_sheet.png", { GetSlotPos(glm::vec2(5.0f, PLAYER_INVENTORY_HEIGHT + 2.5)), 0 });

		furnace_progress->width = 64;
		furnace_progress->height = 64;

		furnace_progress->src = furnace_progress->t->spriteSheet.GetUV("progress_0");

		if (!furnace.GetTag("cooking").IsReal())
			return;


		std::string cookingTag = furnace.GetTag("cooking").value;
		std::string cookingCount = furnace.GetTag("cooking_count").value;
		std::string fuelTag = furnace.GetTag("fuel").value;

		int cTag = std::stoi(cookingTag);
		int fTag = std::stoi(fuelTag);

		furnace_cooking = Data::InventoryItem(std::stoi(cookingTag), std::stoi(cookingCount));
		furnace_fuel = Data::InventoryItem(std::stoi(fuelTag), std::stoi(furnace.GetTag("fuel_count").value));
	}
}

void Inventory::UpdateInventory(bool dontRemoveOutput)
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

			s->id = sl.id;

			s->width = 32;
			s->height = 32;

			sl.front = s;
		}
	}

	// armor
	int id = 38;
	if (isArmor)
	{
		for (int y = 2; y < 5; y++) // 36, 37, 38
		{
			Data::InventoryItem& item = player->playerData.armor[y - 2];

			if (item.type == Data::ItemType::ITEM_NULL)
			{
				id++;
				continue;
			}

			BoxSlot& sl = GetSlot(id);

			if (sl.id == -1)
			{
				id++;
				continue;
			}

			glm::vec3 pos = position + glm::vec3(64 * sl.x, 64 * sl.y, 0);

			ItemUI* s = new ItemUI(item.tag, pos, i, item.count);

			s->id = sl.id;

			s->width = 32;
			s->height = 32;

			sl.front = s;
			id--;
		}
	}

	// crafting (2x2)

	id = 39;
	if (!isThreeByThree)
	{
		for (int y = 1; y < 3; y++) // 39, 40, 41, 42
		{
			for (int x = 1; x < 3; x++)
			{
				Data::InventoryItem& item = crafting[(y - 1) * 2 + (x - 1)];

				if (item.type == Data::ItemType::ITEM_NULL)
				{
					id++;
					continue;
				}

				BoxSlot& sl = GetSlot(id);

				if (sl.id == -1)
				{
					id++;
					continue;
				}

				glm::vec3 pos = position + glm::vec3(64 * sl.x, 64 * sl.y, 0);

				ItemUI* s = new ItemUI(item.tag, pos, i, item.count);

				s->id = sl.id;

				s->width = 32;
				s->height = 32;

				sl.front = s;
				id++;
			}
		}
	}
	else
	{
		// crafting (3x3)

		for (int y = 1; y < 4; y++)
		{
			for (int x = 1; x < 4; x++)
			{
				Data::InventoryItem& item = crafting[(y - 1) * 3 + (x - 1)];

				if (item.type == Data::ItemType::ITEM_NULL)
				{
					id++;
					continue;
				}

				BoxSlot& sl = GetSlot(id);

				if (sl.id == -1)
				{
					id++;
					continue;
				}

				glm::vec3 pos = position + glm::vec3(64 * sl.x, 64 * sl.y, 0);

				ItemUI* s = new ItemUI(item.tag, pos, i, item.count);

				s->id = sl.id;

				s->width = 32;
				s->height = 32;

				sl.front = s;
				id++;
			}
		}
	}

	// furnace

	if (isFurnace)
	{
		Data::InventoryItem item = furnace_cooking;

		if (item.type != Data::ItemType::ITEM_NULL)
		{
			BoxSlot& sl = GetSlot(49);

			if (sl.id != -1)
			{
				glm::vec3 pos = position + glm::vec3(64 * sl.x, 64 * sl.y, 0);

				ItemUI* s = new ItemUI(item.tag, pos, i, item.count);

				s->id = sl.id;

				s->width = 32;
				s->height = 32;

				sl.front = s;
			}
		}

		item = furnace_fuel;

		if (item.type != Data::ItemType::ITEM_NULL)
		{
			BoxSlot& sl = GetSlot(48);

			if (sl.id != -1)
			{
				glm::vec3 pos = position + glm::vec3(64 * sl.x, 64 * sl.y, 0);

				ItemUI* s = new ItemUI(item.tag, pos, i, item.count);

				s->id = sl.id;

				s->width = 32;
				s->height = 32;

				sl.front = s;
			}
		}
	}

	// output

	if (!isFurnace)
	{
		SetCrafting();

		Data::InventoryItem out = CraftingManager::GetInstance()->Craft(stored_crafting);

		output = out;
	}
	else
	{
		std::string out = furnace.GetTag("output").value;
		std::string outCount = furnace.GetTag("outputCount").value;

		if (out != "-1" && out != "data" && outCount != "data")
		{
			output = Data::InventoryItem(std::stoi(out), 1);
			output.count = std::stoi(outCount);
		}
	}

	BoxSlot& sl = GetSlot(90);

	if (sl.id != -1 && output.type != Data::ItemType::ITEM_NULL)
	{
		glm::vec3 pos = position + glm::vec3(64 * sl.x, 64 * sl.y, 0);

		ItemUI* s = new ItemUI(output.tag, pos, i, output.count);

		s->id = sl.id;

		s->width = 32;
		s->height = 32;

		sl.front = s;
	}

	Draw();
}

glm::vec2 Inventory::ConvertToSlotPos(std::string tag_id)
{
	std::vector<std::string> sPos = StringTools::Split(tag_id, ",");

	glm::vec2 start = glm::vec2(std::stoi(sPos[0]), std::stoi(sPos[1]));

	start.x -= 1;
	start.y = PLAYER_INVENTORY_HEIGHT - start.y;

	return start;
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
				*item1 = {};
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

Data::InventoryItem* Inventory::GetItem(int id, glm::vec2 pos)
{
	int armorId = 2 - (id - 36);
	if (id < 36)
		return &player->playerData.inventory[(int)pos.x][(int)pos.y];
	else if (id < 39)
		return &player->playerData.armor[armorId];
	else if (id == 49)
		return &furnace_cooking;
	else if (id == 48)
		return &furnace_fuel;
	else if (id < 90)
		return &crafting[id - 39];
	else if (id == 90)
		return &output;

	return nullptr;
}

void Inventory::Close()
{
	// give back all crafting

	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	for (int i = 0; i < 4; i++)
	{
		Data::InventoryItem item = crafting[i];

		if (item.type == 0)
			continue;

		if (!player->playerData.GiveItem(item)) // inventory full, so drop it
		{
			Camera* c = Game::instance->GetCamera();

			gp->dim->SpawnItem(player->position + c->cameraFront, c->cameraFront, item);
		}

		crafting[i] = {};
	}

	// give back output

	if (output.type != Data::ItemType::ITEM_NULL && !isFurnace)
	{
		if (!player->playerData.GiveItem(output)) // inventory full, so drop it
		{
			Camera* c = Game::instance->GetCamera();

			gp->dim->SpawnItem(player->position + c->cameraFront, c->cameraFront, output);
		}

		output = {};
	}

	if (_dragging)
	{
		// drop item

		if (!player->playerData.GiveItem(stored)) // inventory full, so drop it
		{
			Camera* c = Game::instance->GetCamera();

			gp->dim->SpawnItem(player->position + c->cameraFront, c->cameraFront, stored);
		}

		stored = {};
	}

	_dragging = false;
	delete _draggingItem;
	_draggingItem = nullptr;

	if (isFurnace && player->selectedBlock != nullptr)
	{
		// save the data
		Chunk* c = WorldManager::instance->GetChunk(player->selectedBlock->position.x, player->selectedBlock->position.z);

		int x = player->selectedBlock->chunkPosition.x;
		int y = player->selectedBlock->chunkPosition.y;
		int z = player->selectedBlock->chunkPosition.z;

		furnace.SetTag("cooking", std::to_string(furnace_cooking.type));
		furnace.SetTag("cooking_count", std::to_string(furnace_cooking.count));
		furnace.SetTag("fuel", std::to_string(furnace_fuel.type));
		furnace.SetTag("fuel_count", std::to_string(furnace_fuel.count));

		furnace.SetTag("output", std::to_string(output.type));
		furnace.SetTag("outputCount", std::to_string(output.count));

		c->myData.setBlockData(x, y, z, furnace);
		player->selectedBlock->data = furnace;

		output = {};

		isFurnace = false;
	}

	gp->hud->UpdateHotbar();

}

bool Inventory::SwitchItem(glm::vec3 from, glm::vec3 to, bool one)
{
	BoxSlot& s = GetSlot(to); // to
	BoxSlot& sSlot = GetSlot(from); // from
	Sprite2D* slot = s.slot;
	Sprite2D* startSlot = sSlot.slot;

	_dragging = false;

	if (slot != nullptr && startSlot != nullptr && s.id != -1 && sSlot.id != -1)
	{
		// 9x4

		glm::vec2 start = ConvertToSlotPos(startSlot->tag_id);

		glm::vec2 end = ConvertToSlotPos(slot->tag_id);

		Data::InventoryItem* startItem = GetItem(sSlot.id, start);
		*startItem = stored;
		Data::InventoryItem* endItem = GetItem(s.id, end);

		if (s.id == sSlot.id)
			return false;

		if (s.id == 90) // can't move to output
			return false;

		if (s.id == 48) // fuel
		{
			if (startItem->type == Data::ItemType::ITEM_NULL)
				return false;

			if (startItem->type != Data::ItemType::ITEM_COAL)
				return false;
		}

		// check if we can move it to that slot (armor)

		if (s.id >= 36 && s.id < 39)
		{
			int slot = 2 - (s.id - 36);
			if (!startItem->equipable || slot != startItem->armorSlot)
				return false;
		}

		if (sSlot.id == 90 && endItem->type != Data::ITEM_NULL && endItem->type != stored.type)
		{
			Data::InventoryItem temp = *endItem;
			*endItem = stored;

			*startItem = {};

			if (!player->playerData.GiveItem(temp))
			{
				// drop item

				Gameplay* gp = (Gameplay*)Game::instance->currentScene;

				Camera* c = Game::instance->GetCamera();

				gp->dim->SpawnItem(player->position + c->cameraFront, c->cameraFront, temp);
			}


		}
		else
			ApplyMove(startItem, endItem);

		if (s.id == 48 || s.id == 49 || sSlot.id == 48 || sSlot.id == 49)
		{
			if (player->selectedBlock != nullptr)
				furnace = player->selectedBlock->data;

			furnace.SetTag("cooking", std::to_string(furnace_cooking.type));
			furnace.SetTag("cooking_count", std::to_string(furnace_cooking.count));
			furnace.SetTag("fuel", std::to_string(furnace_fuel.type));
			furnace.SetTag("fuel_count", std::to_string(furnace_fuel.count));

			if (player->selectedBlock != nullptr)
				player->selectedBlock->data = furnace;
		}

		if (sSlot.id == 90 && isFurnace)
		{
			if (player->selectedBlock != nullptr)
				furnace = player->selectedBlock->data;

			furnace.SetTag("output", std::to_string(output.type));
			furnace.SetTag("outputCount", std::to_string(output.count));

			if (player->selectedBlock != nullptr)
				player->selectedBlock->data = furnace;
		}

	}
	else if (startSlot != nullptr)
	{
		glm::vec2 start = ConvertToSlotPos(startSlot->tag_id);

		Camera* c = Game::instance->GetCamera();

		Gameplay* gp = (Gameplay*)Game::instance->currentScene;

		gp->dim->SpawnItem(player->position + c->cameraFront, c->cameraFront, stored);

		return true;
	}

	return true;
}

void Inventory::MouseClick(int button, glm::vec2 pos)
{
	if (!shown)
		return;

	if (button == 0)
	{
		if (!_dragging)
		{
			BoxSlot& sSlot = GetSlot(pos);

			if (sSlot.slot == nullptr)
				return;

			glm::vec2 start = ConvertToSlotPos(sSlot.slot->tag_id);

			Data::InventoryItem* it = GetItem(sSlot.id, start);

			if (it == nullptr || it->type == Data::ITEM_NULL)
				return;


			if (sSlot.id == 90)
			{
				UpdateTable();
				// if shift click, move to inventory

				bool shift = glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

				if (shift)
				{
					if (!isFurnace)
					{
						// craft until we can't anymore

						Data::InventoryItem i = output;

						Data::InventoryItem storedc[3][3];

						for (int y = 0; y < 3; y++)
						{
							for (int x = 0; x < 3; x++)
							{
								storedc[x][y] = stored_crafting[x][y];
							}
						}

						while (i.type != Data::ItemType::ITEM_NULL)
						{
							if (!player->playerData.GiveItem(i)) // inventory full, so we stop
							{
								for (int y = 0; y < 3; y++)
								{
									for (int x = 0; x < 3; x++)
									{
										stored_crafting[x][y] = storedc[x][y];
									}
								}
								break;
							}

							i = CraftingManager::GetInstance()->Craft(stored_crafting);
							UpdateTable();
							SetCrafting();

							for (int y = 0; y < 3; y++)
							{
								for (int x = 0; x < 3; x++)
								{
									storedc[x][y] = stored_crafting[x][y];
								}
							}
						}

						output = i;
						UpdateInventory();
						return;
					}
					else
					{
						// instant take

						if (output.type != Data::ItemType::ITEM_NULL)
						{
							if (player->playerData.GiveItem(output)) // inventory full, so we do nothing
							{
								output = {};
								UpdateInventory();
							}
							return;
						}
					}
				}
			}

			// check if we are dragging a crafting ingredient

			if (sSlot.id >= 39 && sSlot.id < 90)
			{
				output = {};
				RemoveFront(90);
			}

			stored = *it;

			*it = {};


			Gameplay* gp = (Gameplay*)Game::instance->currentScene;

			gp->hud->UpdateHotbar();

			if (isFurnace && sSlot.id == 90)
			{
				output = {};
				RemoveFront(90);
			}


			Texture* _t = Texture::createWithImage("Assets/Textures/items.png", false); // grab from cache

			_draggingItem = new ItemUI(stored.tag, glm::vec3(pos.x - 32, pos.y - 32, 0), _t, stored.count);

			_draggingItem->width = 32;
			_draggingItem->height = 32;

			if (_draggingItem != nullptr)
			{
				_dragging = true;
				_startDrag = Game::instance->GetCursorPos();
			}
		}
		else
		{
			glm::vec3 from = glm::vec3(_startDrag.x, _startDrag.y, 0);
			glm::vec3 to = glm::vec3(pos.x, pos.y, 0);

			BoxSlot& sSlot = GetSlot(_startDrag);
			BoxSlot& eSlot = GetSlot(pos);

			if (eSlot.id == 90)
			{
				SetCrafting();

				Data::InventoryItem out = CraftingManager::GetInstance()->Craft(stored_crafting);

				UpdateTable();

				if (out.type != Data::ItemType::ITEM_NULL)
					output = out;

				if (output.type == Data::ItemType::ITEM_NULL || output.type != stored.type || !stored.stackable)
					return;

				if (stored.count + output.count > 64)
					return;

				stored.count += output.count;

				_draggingItem->SetCount(stored.count);

				output = {};

				UpdateInventory();
				return;
			}

			SwitchItem(from, to);


			Gameplay* gp = (Gameplay*)Game::instance->currentScene;

			gp->hud->UpdateHotbar();

			stored = {};

			_dragging = false;
			delete _draggingItem;
			_draggingItem = nullptr;
		}

	}

	if (button == 1)
	{
		if (_dragging)
		{
			glm::vec3 from = glm::vec3(_startDrag.x, _startDrag.y, 0);
			glm::vec3 to = glm::vec3(pos.x, pos.y, 0);

			BoxSlot& sSlot = GetSlot(from); // from
			BoxSlot& s = GetSlot(to); // to

			_draggingSlot = sSlot.id;

			Sprite2D* startSlot = sSlot.slot;
			Sprite2D* slot = s.slot;

			if (s.id == 90) // can't move to output
				return;

			if (slot == nullptr)
				return;

			glm::vec2 start = ConvertToSlotPos(startSlot->tag_id);
			glm::vec2 end = ConvertToSlotPos(slot->tag_id);

			Data::InventoryItem* startItem = GetItem(sSlot.id, start);
			Data::InventoryItem* endItem = GetItem(s.id, end);

			if (stored.type != endItem->type && endItem->type != Data::ITEM_NULL)
				return;

			if (endItem->count + 1 > 64)
				return;

			stored.count--;

			_draggingItem->SetCount(stored.count);

			if (endItem->type == Data::ITEM_NULL)
			{
				Data::InventoryItem copy = stored;
				copy.count = 1;
				*endItem = copy;
			}
			else
			{
				if (!endItem->stackable)
					return;

				endItem->count++;
			}

			if (s.id == 48 || s.id == 49 || sSlot.id == 48 || sSlot.id == 49)
			{
				if (player->selectedBlock != nullptr)
					furnace = player->selectedBlock->data;

				furnace.SetTag("cooking", std::to_string(furnace_cooking.type));
				furnace.SetTag("cooking_count", std::to_string(furnace_cooking.count));
				furnace.SetTag("fuel", std::to_string(furnace_fuel.type));
				furnace.SetTag("fuel_count", std::to_string(furnace_fuel.count));

				if (player->selectedBlock != nullptr)
					player->selectedBlock->data = furnace;
			}

			if (sSlot.id == 90 && isFurnace)
			{
				if (player->selectedBlock != nullptr)
					furnace = player->selectedBlock->data;

				furnace.SetTag("output", std::to_string(output.type));
				furnace.SetTag("outputCount", std::to_string(output.count));

				if (player->selectedBlock != nullptr)
					player->selectedBlock->data = furnace;
			}

			Gameplay* gp = (Gameplay*)Game::instance->currentScene;

			if (stored.count <= 0)
			{
				stored = {};
				delete _draggingItem;
				_draggingItem = nullptr;
				_dragging = false;
			}

			gp->hud->UpdateHotbar();
		}
	}
}

void Inventory::MouseRelease(int button, glm::vec2 pos)
{

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

	BoxUI::Draw();

	if (_dragging)
	{
		_draggingItem->position = glm::vec3(mouse.x - _draggingItem->width / 2, mouse.y - _draggingItem->height / 2, 0);
		_draggingItem->Draw();

		for(auto& d : _draggingItem->draws)
			draws.push_back(d);
	}

	if (isFurnace)
	{
		Block* selectedBlock = player->selectedBlock;
		if (furnace_progress != nullptr && selectedBlock != nullptr)
		{
			Furnace* f = (Furnace*)selectedBlock;
			float progress = f->tickPerc;

			int p = 0;

			// evenly split among 6 images

			if (progress <= 0.f)
				p = 0;
			else if (progress < 0.166f)
				p = 1;
			else if (progress < 0.333f)
				p = 2;
			else if (progress < 0.5f)
				p = 3;
			else if (progress < 0.666f)
				p = 4;
			else if (progress < 0.833f)
				p = 5;
			else
				p = 6;

			furnace_progress->src = furnace_progress->t->spriteSheet.GetUV("progress_" + std::to_string(p));
			furnace_progress->Draw();

			for (auto& d : furnace_progress->draws)
				draws.push_back(d);
		}
	}

}
