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

	std::vector<Vertex> vertices = {};
	std::vector<unsigned int> indices = {};

	float blockWidth = 128;
	float blockHeight = 128;

public:
	Texture* sheet;

	std::map<int,std::map<int, std::map<int, Block*>>> blocks;

	void AddToDraw(std::vector<Vertex> _v, std::vector<unsigned int> _i);

	Chunk(glm::vec3 pos, Texture* _spr);

	void AddBlock(Block* block);

	void GenerateMesh();

	void Create() override;

	void Draw() override;
};

#endif