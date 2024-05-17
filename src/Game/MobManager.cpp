#include "Scenes/Gameplay.h"
#include "LightingManager.h"
#include <Game.h>

#include "Objects/Base/Zombie.h"
#include "Objects/Base/Pig.h"

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
		a = new Pig(pos);

		gp->AddObject(a);

		mobs.push_back(a);
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

		for (int i = -4; i < 4; i++)
		{
			for (int j = -4; j < 4; j++)
			{
				glm::vec3 pos = glm::vec3(playerChunk->position.x + (16 * i), playerPos.y, playerChunk->position.z + (16 * j));

				Chunk* c = WorldManager::instance->GetChunk(pos.x, pos.z);

				if (c == nullptr)
					continue;

				float r = rand() % 100;

				if (r < 10)
				{

					float distance = glm::distance(playerPos, pos);

					float angle = cam->YawAngleTo(pos);

					if (angle < 100 || distance < 16)
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
				else if (r < 18)
				{
					float distance = glm::distance(playerPos, pos);

					float angle = cam->YawAngleTo(pos);

					if (angle < 100 || distance < 16)
						continue;

					int randomX = rand() % 16;
					int randomZ = rand() % 16;

					int highest = c->GetHighestBlock(randomX, randomZ);

					if (highest <= 0)
						continue;

					int lightLevel = LightingManager::GetInstance()->GetLightLevel(glm::vec3(c->position.x + randomX, highest, c->position.z + randomZ));

					if (lightLevel < 5)
						continue;


					int type = c->GetBlock(randomX, highest, randomZ);

					if (type == GRASS)
						CreateMob(AI_Type::TPig, glm::vec3(pos.x + randomX, highest + 1, pos.z + randomZ));
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

				if (r < 8)
				{
					Data::InventoryItem it{ Data::ITEM_COAL, 1 };

					gp->dim->SpawnItem(mob->position, it);
				}
				break;
			}
			case AI_Type::TPig:
				int count = rand() % 3 + 1;

				for (int i = 0; i < count; i++)
				{
					Data::InventoryItem it{ Data::ITEM_UNCOOKED_PORK, 1 };

					gp->dim->SpawnItem(mob->position, it);
				}
				break;
			}

			RemoveMob(mob);
			break;
		}

		float r = rand() % 35 + 20;

		if (mob->lastNoise + r < glfwGetTime())
			mob->Noise();
	}
}
