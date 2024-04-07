#include "DroppedItemManager.h"
#include <Game.h>
#include "Scenes/Gameplay.h"

DroppedItemManager::~DroppedItemManager()
{
	for (int i = 0; i < items.size(); i++)
	{
		delete items[i];
	}

	items.clear();
}

void DroppedItemManager::SpawnItem(glm::vec3 pos, glm::vec3 front, Data::InventoryItem i, bool stackable)
{
	Texture* t = Texture::createWithImage("Assets/Textures/items.png", false); // grab from cache

	DroppedItem* item = new DroppedItem(pos, t, i, stackable);

	item->Launch(front, 20.0f, 0.0f);

	AddItem(item);
}

void DroppedItemManager::SpawnItem(glm::vec3 pos, Data::InventoryItem i, bool stackable)
{
	Texture* t = Texture::createWithImage("Assets/Textures/items.png", false); // grab from cache

	DroppedItem* item = new DroppedItem(pos, t, i, stackable);

	AddItem(item);
}

void DroppedItemManager::AddItem(DroppedItem* item)
{
	items.push_back(item);

	Gameplay* g = (Gameplay*)Game::instance->currentScene;

	g->DelayedAddObject(item);
}

void DroppedItemManager::RemoveItem(DroppedItem* item)
{
	for (int i = 0; i < items.size(); i++)
	{
		if (items[i] == item)
		{
			items.erase(items.begin() + i);
			Gameplay* g = (Gameplay*)Game::instance->currentScene;
			g->RemoveObject(item);

			delete item;
			break;
		}
	}
}

void DroppedItemManager::Update()
{
	Gameplay* g = (Gameplay*)Game::instance->currentScene;

	Player* player = g->player;

	for (int i = 0; i < items.size(); i++)
	{
		DroppedItem* item = items[i];

		if (glfwGetTime() - item->lifeTime > 240) // expire after 4 minutes
		{
			RemoveItem(item);
			break;
		}

		float distance = glm::distance(player->position - glm::vec3(0,1.8,0), item->position);

		if (distance <= 1.0f) // pickup range
		{
			Data::InventoryItem i = item->item;

			player->playerData.GiveItem(i);

			g->hud->UpdateHotbar();
			
			RemoveItem(item);
			break;
		}

		if (item->item.stackable)
			for (int i2 = 0; i2 < items.size(); i2++) // merge (o^2)
			{
				if (i == i2)
					continue;

				DroppedItem* item2 = items[i2];

				if (item->item.type == item2->item.type && glm::distance(item->position, item2->position) <= 1.0f)
				{
					if (item->item.count + item2->item.count > 64)
					{
						int diff = (item->item.count + item2->item.count) - 64;

						item->item.count = diff;
						item2->item.count = 64;
					}
					else
					{
						item2->item.count += item->item.count;
						item->item = {};
					}

					RemoveItem(item2);
					break;
				}
			}
	}
}
