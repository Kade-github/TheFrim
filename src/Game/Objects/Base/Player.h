#ifndef _PLAYER_H
#define _PLAYER_H

#include "Entity.h"
#include "Chunk.h"

class Player : public Entity
{
public:
	Player(glm::vec3 pos);

	void Draw() override;
};

#endif