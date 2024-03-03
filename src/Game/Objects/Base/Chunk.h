#ifndef _CHUNK_H
#define _CHUNK_H

#include <glm/glm.hpp>
#include "Block.h"
#include <map>
#include <mutex>
#include <OpenGL/Texture.h>
#include <Objects/GameObject.h>
#include "../../Data/World.h"
#include <functional>

class Chunk : public GameObject
{
	unsigned int VBO, VAO, EBO;

	bool generatedVAO = false;

	// final vertices and indices
	std::vector<VVertex> vertices = {};
	std::vector<unsigned int> indices = {};

	float blockWidth = 128;
	float blockHeight = 128;

public:
	Texture* sheet = NULL;

	bool isLoaded = false;
	bool rendered = false;

	Data::Chunk data;

	Chunk* frontChunk = NULL;
	Chunk* backChunk = NULL;
	Chunk* leftChunk = NULL;
	Chunk* rightChunk = NULL;

	Block* blocks[16][16][256];

	Chunk(glm::vec3 pos, Texture* _spr);

	Block* CreateBlock(int type, glm::vec3 _pos);

	bool DoesBlockExist(int x, int y, int z);
	Block* GetBlock(float x, float y, float z);
	Block* GetTopBlock(int x, int z);

	void UpdateBlocks(Data::Chunk data);
	void UpdateBlockFace(Block* b);

	void RemoveBlock(int x, int y, int z);
	void PlaceBlock(int x, int y, int z, int type);

	void DeloadBlocks();
	void CleanBlocks();

	void GenerateMesh();
	void DeloadMesh();

	void Create() override;

	void Draw() override;
};

#endif