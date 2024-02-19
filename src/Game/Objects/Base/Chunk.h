#ifndef _CHUNK_H
#define _CHUNK_H

#include "Block.h"
#include <map>
#include <mutex>
#include "../../../Engine/OpenGL/Texture.h"
#include "../../../Engine/Objects/GameObject.h"

class Chunk : public GameObject
{
	unsigned int VBO, VAO, EBO;

	// final vertices and indices

	std::vector<VVertex> vertices = {};
	std::vector<unsigned int> indices = {};

	float blockWidth = 128;
	float blockHeight = 128;

public:
	Texture* sheet;

	Block* blocks[16][256][16] = {};

	void AddToDraw(std::vector<VVertex> _v, std::vector<unsigned int> _i);

	Chunk(glm::vec3 pos, Texture* _spr);

	void AddBlock(Block* block);

	void GenerateMesh();

	void Create() override;

	void Draw() override;
};

#endif