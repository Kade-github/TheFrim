#ifndef _LIGHTINGMANAGER_H
#define _LIGHTINGMANAGER_H

#include <glm/glm.hpp>
#include <vector>
#include "Objects/Base/Chunk.h"

struct Light {
	glm::vec3 position;
	int strength;
};

struct Sun {
	float angle;
	glm::vec3 color;
	int strength = 10;
};

class LightingManager
{
	static LightingManager* instance;
	LightingManager() {}
public:
	std::vector<Light> lights;

	static LightingManager* GetInstance()
	{
		if (!instance)
			instance = new LightingManager();

		return instance;
	}

	Sun sun;

	void SunColor();
	void SunUpdate();

	void ClearLights()
	{
		lights.clear();
	}

	void RefreshShadows();

	void AddLight(glm::vec3 pos, int strength);

	void RemoveLight(glm::vec3 pos);

	Light& GetLight(glm::vec3 pos)
	{
		for (int i = 0; i < lights.size(); i++)
		{
			if (lights[i].position == pos)
			{
				return lights[i];
			}
		}
	}

	int GetLightLevel(glm::vec3 pos);

};

#endif