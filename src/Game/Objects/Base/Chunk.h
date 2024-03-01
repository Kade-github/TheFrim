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

	std::vector<unsigned int> indices = {};

	float blockWidth = 128;
	float blockHeight = 128;

public:

	std::vector<VVertex> vertices = {};

	Data::Chunk* data;
	Data::Chunk* forwardC;
	Data::Chunk* backwardC;
	Data::Chunk* leftC;
	Data::Chunk* rightC;

	Texture* sheet = NULL;

	bool isLoaded = false;
	bool rendered = false;
	bool needData = true;
	bool needReload = false;

	std::vector<Block*> blocks = {};

	void AddToDraw(std::vector<VVertex> _v, std::vector<unsigned int> _i);

	Chunk(glm::vec3 pos, Texture* _spr);

	int getTopBlock(float x, float z);
	int doesBlockExist(float x, float y, float z);

	void GenerateMesh();

	void UploadMesh();
	void Clean();
	void UnloadMesh();

	void Reload();

	void Create() override;

	void Draw() override;
};

#endif