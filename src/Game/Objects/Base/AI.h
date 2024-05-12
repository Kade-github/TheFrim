#ifndef _AI_H
#define _AI_H

#include "Entity.h"

enum AI_Type {
	TZombie = 0,
	TPig = 1
};

class AI : public Entity
{
	bool DoesBlockExist(glm::vec3 pos);
	bool CheckBlock(glm::vec3 from, glm::vec3 pos);

	bool nextPath = false;
	float lastPosition = 0;
	float damageCooldown = 0.0f;
public:
	float redness = 0.0f;
	bool dead = false;
	bool debug = false;
	float lastSeen = 0;

	float health = 10;

	AI_Type type = TZombie;

	float aiYaw = 0;
	float aiPitch = 0;

	AI(glm::vec3 pos) : Entity(pos) {}

	std::vector<glm::vec3> path;

	glm::vec3 target;
	glm::vec3 GenerateRandomPosition(float distance);

	void MoveTo(glm::vec3 pos);

	void MoveToRandom();

	void Hurt(float damage, glm::vec3 from);

	bool IsPositionInMe(glm::vec3 pos);

	void Draw() override;
};

#endif