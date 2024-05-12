#ifndef _MOBMANAGER_H
#define _MOBMANAGER_H

#include "Objects/Base/AI.h"

class MobManager
{
public:
	float lastWave = 0;

	std::vector<AI*> mobs = {};

	void CreateMob(AI_Type mob, glm::vec3 pos);
	void RemoveMob(AI* mob);

	void RemoveAllMobs();

	void Update();
};

#endif