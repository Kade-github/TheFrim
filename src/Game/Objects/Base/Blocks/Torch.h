#ifndef _TORCHBLOCK_H
#define _TORCHBLOCK_H

#include "../Block.h"

class Torch : public Block
{
public:
	Torch(glm::vec3 _position);

	void Destroy() override;

	void LoadModel() override;

	void Mo() override;
};

#endif