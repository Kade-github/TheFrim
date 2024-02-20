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

	// final vertices and indices

	std::vector<VVertex> vertices = {};
	std::vector<unsigned int> indices = {};

	float blockWidth = 128;
	float blockHeight = 128;

public:
	Texture* sheet;

	bool isLoaded = false;

	Data::Chunk* forward;
	Data::Chunk* backward;
	Data::Chunk* left;
	Data::Chunk* right;

	std::vector<Block*> blocks;

	void AddToDraw(std::vector<VVertex> _v, std::vector<unsigned int> _i);

	Chunk(glm::vec3 pos, Texture* _spr);

	void GenerateMesh(Data::Chunk c);

	void UploadMesh();
	void UnloadMesh();

	void Create() override;

	void Draw() override;
};

#endif