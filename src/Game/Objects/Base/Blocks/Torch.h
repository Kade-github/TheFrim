#ifndef _TORCHBLOCK_H
#define _TORCHBLOCK_H

#include "../Block.h"
#include "../Chunk.h"

class Torch : public Block
{
public:
	Chunk* currentChunk;
	int _facing = 0;

	bool broken = false;

	Torch(glm::vec3 _position);

	void Destroy() override;

	void LoadModel() override;

	void Mo() override;

	bool Update(int tick) override;
};

#endif