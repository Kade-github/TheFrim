#ifndef _TORCHBLOCK_H
#define _TORCHBLOCK_H

#include "../Block.h"

class Torch : public Block
{
public:
	Torch(glm::vec3 _position) : Block(_position, BlockType::TORCH) {
		position = _position;

		soundType = SoundType::S_STONE;
		toughness = 9.0f;
		isModel = true;
		transparent = true;
		collidable = false;
	}

	void Destroy() override;

	void LoadModel() override;

	void Mo() override;
};

#endif