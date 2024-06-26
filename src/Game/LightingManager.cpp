#include "LightingManager.h"
#include "WorldManager.h"
#include <glad/glad.h>
#include <Game.h>
#include "Data/Settings.h"
#include "Scenes/Gameplay.h"

LightingManager* LightingManager::instance = nullptr;

bool IsLightBlocked(glm::vec3 start, glm::vec3 end)
{
	// if the y difference is less than 2, we ignore the raycast (might create some light leaks, but it's not a big deal)

	if (abs(start.y - end.y) < 2)
		return false;


	float progress = 0;
	glm::vec3 direction = glm::normalize(end - start);

	while (progress < 1)
	{
		glm::vec3 pos = start + (direction * progress);

		Chunk* c = WorldManager::instance->GetChunk(pos.x, pos.z);

		if (c)
		{
			if (c->DoesBlockExist(pos.x,pos.y,pos.z))
				return true;
		}

		progress += 0.1f;
	}

	return false;
}

void LightingManager::SunColor()
{
	static float caveLerp = 0;
	glm::vec3 peak = glm::vec3(0.64f, 0.7f, 0.7f);
	glm::vec3 dawn = glm::vec3(0.42f, 0.43f, 0.56f);
	glm::vec3 night = glm::vec3(0.05f, 0.06f, 0.09f);

	glm::vec3 cave = glm::vec3(0.1f, 0.1f, 0.1f);

	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	Camera* cam = Game::instance->GetCamera();

	int playerY = gp->player->position.y;
	Chunk* c = WorldManager::instance->GetChunk(gp->player->position.x, gp->player->position.z);
	int highestBlock = 0;
	if (c != nullptr)
		highestBlock = c->GetHighestBlock(gp->player->position.x, gp->player->position.z, true);

	float angle = sun.angle;

	if (angle < 180)
	{
		// day

		float progress = angle / 180;

		// dawn to day (0->0.25)

		if (progress < 0.25f)
		{
			float p = progress / 0.25f;

			sun.color = glm::mix(dawn, peak, p);
			sun.strength = std::lerp(5, 10, p);
		}
		else if (progress < 0.75)
		{
			sun.color = peak;
			sun.strength = 10;
		}
		else if (progress > 0.75f) // go to dawn again
		{
			float p = (progress - 0.75f) / 0.25f;

			sun.color = glm::mix(peak, dawn, p);
			sun.strength = std::lerp(10, 5, p);
		}

	}
	else
	{
		// night

		float progress = (angle - 180) / 180;

		// dawn to night (0->0.25)

		if (progress < 0.25f)
		{
			float p = progress / 0.25f;

			sun.color = glm::mix(dawn, night, p);
			sun.strength = std::lerp(5, 2, p);
		}
		else if (progress < 0.75)
		{
			sun.color = night;
			sun.strength = 2;
		}
		else if (progress > 0.75f) // go to dawn again
		{
			float p = (progress - 0.75f) / 0.25f;

			sun.color = glm::mix(night, dawn, p);
			sun.strength = std::lerp(2, 5, p);
		}
	}

    float distance = (float)highestBlock - (float)playerY;

	if (cam->position.y > 170.0f)
	{
		distance = 5.0f;
	}

    float mixProgress = distance / 5.0f;

    if (mixProgress > 1.0f)
        mixProgress = 1.0f;

    if (mixProgress < 0.0f)
        mixProgress = 0.0f;

    static float lastMax = 0.0f;

	if (mixProgress > 0)
	{

		if (caveLerp < 1)
			caveLerp += Game::instance->deltaTime;

		sun.color = glm::mix(sun.color, cave, std::lerp(0, mixProgress, caveLerp));
        lastMax = mixProgress;
	}
	else if (caveLerp > 0) {
        caveLerp -= Game::instance->deltaTime;

        sun.color = glm::mix(sun.color, cave, std::lerp(0, lastMax, caveLerp));
    }

    sun.color *= Settings::instance->brightness;

	glClearColor(sun.color.x, sun.color.y, sun.color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void LightingManager::SunUpdate()
{
	if (Hud::GamePaused)
		return;

	if (nextFrameRefresh)
	{
		RefreshShadows();
		nextFrameRefresh = false;
	}

	static float lastSunStrength = 10;

	sun.angle += Game::instance->deltaTime * 0.25;

	if (sun.angle >= 360)
		sun.angle = 0;

	if (std::abs(lastSunStrength - sun.strength) >= 2)
	{
		nextFrameRefresh = true;
		lastSunStrength = sun.strength;
	}
}


void LightingManager::RefreshShadows()
{
	auto gp = (Gameplay*)Game::instance->currentScene;
	// Get all regions

	for (auto& c : gp->allChunks)
	{
		float dist = glm::distance(glm::vec3(c->position.x, 0, c->position.z), glm::vec3(gp->player->position.x, 0, gp->player->position.z));
		if (c->isRendered && dist <= 64)
		{
			c->isShadowLoaded = false;
			gp->QueueShadow(c);
		}

	}
}

void LightingManager::AddLight(glm::vec3 pos, int strength)
{
	lights.push_back({ glm::vec3((int)pos.x, (int)pos.y, (int)pos.z), strength});

	nextFrameRefresh = true;
}

void LightingManager::RemoveLight(glm::vec3 pos)
{
	for (int i = 0; i < lights.size(); i++)
	{
		if (glm::vec3((int)pos.x, (int)pos.y, (int)pos.z) == lights[i].position)
		{
			lights.erase(lights.begin() + i);

			nextFrameRefresh = true;
			break;
		}
	}
}
int LightingManager::GetLightLevel(glm::vec3 pos)
{
	int level = sun.strength;

	Chunk* c = WorldManager::instance->GetChunk(pos.x, pos.z);

	if (c == nullptr)
	{
		return 0;
	}

	int highestBlock = c->GetHighestBlock(pos.x, pos.z);

	if (highestBlock > 0 && highestBlock > pos.y)
	{
		level -= (int)((highestBlock - pos.y));

		if (level < 0)
			level = 0;
	}

	int oChunkHighest = 0;
	bool l = false, r = false, f = false, b = false;

	// get left block
	Chunk* left = WorldManager::instance->GetChunk(pos.x - 1, pos.z);

	bool isLow = true;

	if (left != nullptr)
	{
		int highestBlock = left->GetHighestBlock(pos.x - 1, pos.z, true);
		oChunkHighest = highestBlock;
		isLow = highestBlock > 0 && highestBlock > pos.y;
		if (highestBlock > pos.y)
			l = true;
	}

	// get right block

	Chunk* right = WorldManager::instance->GetChunk(pos.x + 1, pos.z);

	if (right != nullptr)
	{
		int highestBlock = right->GetHighestBlock(pos.x + 1, pos.z, true);
		oChunkHighest = highestBlock;
		isLow = highestBlock > 0 && highestBlock > pos.y;
		if (highestBlock > pos.y)
			r = true;
	}

	// get front block

	Chunk* front = WorldManager::instance->GetChunk(pos.x, pos.z + 1);

	if (front != nullptr)
	{
		int highestBlock = front->GetHighestBlock(pos.x, pos.z + 1, true);
		oChunkHighest = highestBlock;
		isLow = highestBlock > 0 && highestBlock > pos.y;
		if (highestBlock > pos.y)
			f = true;
	}

	// get back block

	Chunk* back = WorldManager::instance->GetChunk(pos.x, pos.z - 1);

	if (back != nullptr)
	{
		int highestBlock = back->GetHighestBlock(pos.x, pos.z - 1, true);
		oChunkHighest = highestBlock;
		isLow = highestBlock > 0 && highestBlock > pos.y;
		if (highestBlock > pos.y)
			b = true;
	}

	if (level < 3)
		level = 3;


	for (int i = 0; i < lights.size(); i++)
	{
		float distance = glm::distance(lights[i].position, pos);
		float strength = lights[i].strength;

		if (distance < strength)
		{
			level += (int)(strength - distance);

			if (level > 10)
			{
				level = 10;
				break;
			}
		}
	}


	return level;
}