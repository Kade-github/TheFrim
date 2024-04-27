#ifndef _FALLINGBLOCK_H
#define _FALLINGBLOCK_H

#include "Entity.h"

class FallingBlock : public Entity
{
	unsigned int VAO, VBO, EBO;
public:

	int blockType = 0;

	std::vector<GameObject::VVertex> vertices;
	std::vector<unsigned int> indices;

	Texture* txp;

	FallingBlock(glm::vec3 _pos, Texture* t, std::vector<GameObject::VVertex> v, std::vector<unsigned int> i, int _blockType);

	void Draw() override;
};

#endif