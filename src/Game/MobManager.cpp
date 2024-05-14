#include "Scenes/Gameplay.h"
#include "LightingManager.h"
#include <Game.h>

#include "Objects/Base/Zombie.h"

void MobManager::CreateMob(AI_Type mob, glm::vec3 pos)
{
	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	AI* a = nullptr;
	switch (mob)
	{
	case AI_Type::TZombie:
		a = new Zombie(pos);

		gp->AddObject(a);

		mobs.push_back(a);
		break;
	case AI_Type::TPig:
		break;
	}
}

void MobManager::RemoveMob(AI* mob)
{
	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	mob->Destroy();

	gp->RemoveObject(mob);

	mobs.erase(std::remove(mobs.begin(), mobs.end(), mob), mobs.end());
}

void MobManager::RemoveAllMobs()
{
	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	for (AI* mob : mobs)
	{
		gp->RemoveObject(mob);
	}

	mobs.clear();
}

void MobManager::Update()
{
	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	bool isNight = LightingManager::GetInstance()->sun.angle > 180.0f;

	glm::vec3 playerPos = gp->player->position;

	Camera* cam = Game::instance->GetCamera();

	// spawn mobs around player
	if (glfwGetTime() - lastWave > 30 && mobs.size() < 45)
	{
		lastWave = glfwGetTime();

		Chunk* playerChunk = WorldManager::instance->GetChunk(playerPos.x, playerPos.z);

		for(int i = -4; i < 4; i++)
		{
			for (int j = -4; j < 4; j++)
			{
				if (rand() % 100 < 15)
				{

					glm::vec3 pos = glm::vec3(playerChunk->position.x + (16 * i), playerPos.y, playerChunk->position.z + (16 * j));

					float distance = glm::distance(playerPos, pos);

					float angle = cam->YawAngleTo(pos);

					if (angle < 100 || distance < 16)
						continue;

					// loop through all subchunks

					Chunk* c = WorldManager::instance->GetChunk(pos.x, pos.z);

					if (c == nullptr)
						continue;

					for (auto& sbc : c->subChunks)
					{
						int randomX = rand() % 16;
						int randomZ = rand() % 16;

						if (c->DoesBlockExist(randomX, sbc.y + 2, randomZ))
							continue;

						if (c->DoesBlockExist(randomX, sbc.y + 1, randomZ))
							continue;

						if (!c->DoesBlockExist(randomX, sbc.y, randomZ))
							continue;

						int lightLevel = LightingManager::GetInstance()->GetLightLevel(glm::vec3(c->position.x + randomX, sbc.y, c->position.z + randomZ));

						if (lightLevel < 5)
							CreateMob(AI_Type::TZombie, glm::vec3(pos.x + randomX, sbc.y, pos.z + randomZ));
					}
				}
			}
		}
	}

	for (AI* mob : mobs)
	{
		float angle = cam->YawAngleTo(mob->position);

		if (angle > 100 && glfwGetTime() - mob->lastSeen > 140) // not visible
		{
			RemoveMob(mob);
			break;
		}
		else if (angle > 100)
			mob->lastSeen = glfwGetTime();

		if (mob->dead)
		{
			// drops

			switch (mob->type)
			{
				case AI_Type::TZombie:
				{
					int r = rand() % 100;

					if (r < 15)
					{
						Data::InventoryItem it{ Data::ITEM_GOLD_INGOT, 1 };

						gp->dim->SpawnItem(mob->position, it);
					}
					else if (r < 45)
					{
						Data::InventoryItem it{ Data::ITEM_IRON_INGOT, 1 };

						gp->dim->SpawnItem(mob->position, it);
					}
					break;
				}
				case AI_Type::TPig:
					int count = rand() % 3 + 1;

					Data::InventoryItem it{ Data::ITEM_UNCOOKED_PORK, count };

					gp->dim->SpawnItem(mob->position, it);
					break;
			}

			RemoveMob(mob);
		}
	}
}