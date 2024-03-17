#include "LightingManager.h"
#include "WorldManager.h"

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

void LightingManager::RefreshShadows()
{
	// Get all regions

	for (auto& r : WorldManager::instance->regions)
	{
		// Get all chunks in that region

		for (auto& c : r.chunks)
		{
			if (c->isRendered)
			{
				c->RenderSubChunksShadow(); // Render their shadows
				c->SetShadowBuffer();
			}
		}
	}
}

void LightingManager::AddLight(glm::vec3 pos, int strength)
{
	lights.push_back({ glm::vec3((int)pos.x, (int)pos.y, (int)pos.z), strength});

	RefreshShadows();
}

void LightingManager::RemoveLight(glm::vec3 pos)
{
	for (int i = 0; i < lights.size(); i++)
	{
		if (glm::vec3((int)pos.x, (int)pos.y, (int)pos.z) == lights[i].position)
		{
			lights.erase(lights.begin() + i);
			break;
		}
	}

	RefreshShadows();
}
int LightingManager::GetLightLevel(glm::vec3 pos)
{
	int level = 4;

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