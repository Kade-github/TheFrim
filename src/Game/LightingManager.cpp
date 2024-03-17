#include "LightingManager.h"
#include "WorldManager.h"
#include <glad/glad.h>
#include <Game.h>

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
	glm::vec3 peak = glm::vec3(0.64f, 0.7f, 0.7f);
	glm::vec3 dawn = glm::vec3(0.83f, 0.6f, 0.28f);
	glm::vec3 night = glm::vec3(0.05f, 0.06f, 0.09f);

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

	glClearColor(sun.color.x, sun.color.y, sun.color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void LightingManager::SunUpdate()
{
	static float lastSunStrength = 10;

	sun.angle += Game::instance->deltaTime;

	if (sun.angle >= 360)
		sun.angle = 0;

	if (std::abs(lastSunStrength - sun.strength) >= 2)
	{
		RefreshShadows();
		lastSunStrength = sun.strength;
	}
}

void LightingManager::RefreshShadows()
{
	// Get all regions

	for (auto& r : WorldManager::instance->regions)
	{
		// Get all chunks in that region

		for (auto& c : r.chunks)
		{
			if (c->isLoaded)
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
	int level = sun.strength;

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