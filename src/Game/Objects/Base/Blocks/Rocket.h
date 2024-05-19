#ifndef _ROCKET_H
#define _ROCKET_H

#include "../Block.h"
#include "../Chunk.h"

class Rocket : public Block
{
public:
	Chunk* currentChunk;

	Rocket(glm::vec3 _position);

	void LoadModel() override;

	void Mo() override;

	void OnInteract() override;

	bool Update(int tick) override;
};

#endif