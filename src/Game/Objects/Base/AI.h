#ifndef _AI_H
#define _AI_H

#include "Entity.h"

class AI : public Entity
{
	bool DoesBlockExist(glm::vec3 pos);
	bool CheckBlock(glm::vec3 from, glm::vec3 pos);

	bool nextPath = false;
	float lastPosition = 0;

public:
	bool debug = false;

	float aiYaw = 0;
	float aiPitch = 0;

	AI(glm::vec3 pos) : Entity(pos) {}

	std::vector<glm::vec3> path;

	glm::vec3 target;
	glm::vec3 GenerateRandomPosition(float distance);

	void MoveTo(glm::vec3 pos);

	void MoveToRandom();

	void Draw() override;
};

#endif