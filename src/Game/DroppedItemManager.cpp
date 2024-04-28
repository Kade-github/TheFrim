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

void DroppedItemManager::SpawnItem(glm::vec3 pos, glm::vec3 front, Data::InventoryItem i, float force, float upForce)
{
	Texture* t = Texture::createWithImageExtra("Assets/Textures/items.png", "nonFlipped"); // grab from cache
	t->spriteSheet.Load("Assets/Textures/items.xml", t->width, t->height);

	DroppedItem* item = new DroppedItem(pos, t, i);

	item->Launch(front, force, upForce);

	AddItem(item);
}

void DroppedItemManager::SpawnItem(glm::vec3 pos, Data::InventoryItem i)
{
	Texture* t = Texture::createWithImageExtra("Assets/Textures/items.png", "nonFlipped"); // grab from cache
	t->spriteSheet.Load("Assets/Textures/items.xml", t->width, t->height);

	DroppedItem* item = new DroppedItem(pos, t, i);

	AddItem(item);
}

void DroppedItemManager::AddItem(DroppedItem* item)
{
	static int itemId = 0;
	items.push_back(item);

	Gameplay* g = (Gameplay*)Game::instance->currentScene;

	item->id = itemId++;

	g->DelayedAddObject(item);
}

void DroppedItemManager::RemoveItem(DroppedItem* item)
{
	for (int i = 0; i < items.size(); i++)
	{
		if (items[i]->id == item->id)
		{
			items.erase(items.begin() + i);
			Gameplay* g = (Gameplay*)Game::instance->currentScene;
			g->RemoveObject(item);

			delete item;
			item = nullptr;
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

		float time = glfwGetTime() - item->lifeTime;

		if (time < 0.25f || item->delayed)
			continue;

		if (time > 240.0f) // expire after 4 minutes
		{
			RemoveItem(item);
			break;
		}

		float distance = glm::distance(player->position - glm::vec3(0,1.8,0), item->position);

		if (distance <= 1.35f && (
			(int)item->position.y == (int)(player->position.y - 1) ||
			(int)item->position.y == (int)(player->position.y) ||
			(int)item->position.y == (int)(player->position.y + 1))) // pickup range
		{
			Data::InventoryItem it = item->item;

			player->playerData.GiveItem(it);

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

				float lifeTime = glfwGetTime() - item2->lifeTime;

				if (lifeTime < 0.25f || item2->delayed)
					continue;

				if (item->item.type == item2->item.type && glm::distance(item->position, item2->position) <= 0.5f)
				{
					if (item->item.count + item2->item.count > 64)
					{
						int diff = (item->item.count + item2->item.count) - 64;

						item2->item.count = diff;
						item->item.count = 64;
					}
					else
					{
						item->item.count += item2->item.count;
					}

					RemoveItem(item2);
					break;
				}
			}
	}
}
