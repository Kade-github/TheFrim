#ifndef _PLAYER_H
#define _PLAYER_H

#include "Entity.h"
#include "Chunk.h"

class Player : public Entity
{
public:
	float jumpCooldown = 0;

	Player(glm::vec3 pos);

	void Draw() override;

	bool freeCam = false;

	bool ClosestAirBlock(glm::vec3& pos);

	void MouseClick(int button, glm::vec2 mPos) override;
};

#endif