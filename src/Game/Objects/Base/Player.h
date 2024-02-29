#ifndef _PLAYER_H
#define _PLAYER_H

#include "Entity.h"
#include "Chunk.h"

class Player : public Entity
{
public:
	Player(glm::vec3 pos);

	void Draw() override;

	glm::vec3 ForwardRay();

	void MouseClick(int button, glm::vec2 mPos) override;
};

#endif